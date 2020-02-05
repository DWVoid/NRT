#include "Msg/Tunnels.h"
#if __has_include(<Windows.h>)
#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00
#endif
#include <boost/asio.hpp>
#include "Cfx/Threading/SpinLock.h"

using boost::asio::ip::tcp;

namespace {
    struct IoService {
        inline static boost::asio::io_context Context{};
        inline static std::atomic_int Counter{0};
        using Guard = boost::asio::executor_work_guard<decltype(Context.get_executor())>;
        inline static std::aligned_storage_t<sizeof(Guard), alignof(Guard)> Stg;

        IoService() noexcept {
            if (Counter.fetch_add(1)==0) {
                (void) new(&Stg)Guard(Context.get_executor());
                std::thread([]() { Context.run(); }).detach();
            }
        }

        ~IoService() noexcept {
            if (Counter.fetch_sub(1)==1) {
                const auto stg = reinterpret_cast<Guard*>(&Stg);
                stg->reset();
                stg->~Guard();
            }
        }

        static auto& Get() noexcept { return Context; }
    };

    struct TcpMessageHeader {
        TcpMessageHeader() noexcept = default;

        [[nodiscard]] uint32_t Type() const noexcept {
            return le32toh(*reinterpret_cast<const uint32_t*>(Data));
        }

        [[nodiscard]] uint32_t Length() const noexcept {
            return le32toh(*reinterpret_cast<const uint32_t*>(Data+4));
        }

        std::byte Data[8]{};
    };

    auto CreateAbortedIo() noexcept { return std::make_exception_ptr(AbortedIo{}); }

    class TcpIo {
    public:
        explicit TcpIo(boost::asio::io_service& io_service)
                :_Socket(io_service) { (void) _Srv; }

        Future<boost::system::error_code> WriteAsync(uint32_t length, const std::byte* content) noexcept {
            Promise<boost::system::error_code> complete{};
            auto fut = complete.GetFuture();
            async_write(_Socket, boost::asio::const_buffer{content, length},
                    boost::asio::transfer_exactly(length),
                    [complete = std::move(complete)](auto&& ec, auto&&) mutable noexcept {
                        complete.SetValueUnsafe(ec);
                    });
            return fut;
        }

        Future<boost::system::error_code> ReadAsync(uint32_t length, std::byte* content) noexcept {
            Promise<boost::system::error_code> complete{};
            auto fut = complete.GetFuture();
            async_read(_Socket, boost::asio::mutable_buffer{content, length},
                    boost::asio::transfer_exactly(length),
                    [complete = std::move(complete)](auto&& ec, auto&&) mutable noexcept {
                        complete.SetValueUnsafe(ec);
                    });
            return fut;
        }

        auto& Native() noexcept { return _Socket; }

        void Shutdown() {
            _Socket.shutdown(boost::asio::socket_base::shutdown_both);
        }
    private:
        IoService _Srv{};
        tcp::socket _Socket;
    };

    class TcpConnection : public IEndPointTcp, TcpIo {
    public:
        explicit TcpConnection(boost::asio::io_service& io_service): TcpIo(io_service) {}

        static auto Create(boost::asio::io_context& context) { return std::make_unique<TcpConnection>(context); }

        tcp::socket& Socket() noexcept { return Native(); }

        Future<void> Send(InMessage& msg) override {
            auto stm = Temp::New<SendStateMachine>();
            return stm->Start(msg, this);
        }

        Future<OutMessage> Wait() override {
            auto stm = Temp::New<WaitStateMachine>(this);
            return stm->Start();
        }

        Future<void> Stop() override {
            Shutdown();
            _Lock.Enter();
            auto last = std::move(Order);
            _Lock.Leave();
            return last;
        }

    private:
        struct WaitStateMachine {
            explicit WaitStateMachine(TcpConnection* _this) noexcept
                    :This(_this) { }

            void MoveNext(const uint32_t step) noexcept {
                switch (step) {
                case 0:
                    This->ReadAsync(8, Head.Data).Then([this](auto&& f) noexcept {
                        MoveNext((f.Get() ? -1 : 1));
                    });
                    return;
                case 1:Out.Type = Head.Type();
                    Out.Length = Head.Length();
                    Out.Data = decltype(Out.Data)(Temp::Allocator<std::byte>().allocate(Head.Length()),
                            {Head.Length()});
                    This->ReadAsync(Head.Length(), Out.Data.get()).Then([this](auto&& f) noexcept {
                        MoveNext((f.Get() ? -1 : 2));
                    });
                    return;
                case 2: Complete.SetValueUnsafe(std::move(Out));
                    break;
                default: Complete.SetExceptionUnsafe(CreateAbortedIo());
                }
                Temp::Delete(this);
            }

            Future<OutMessage> Start() noexcept {
                MoveNext(0);
                return Complete.GetFuture();
            }

            Promise<OutMessage> Complete{};
            TcpMessageHeader Head{};
            TcpConnection* This;
            OutMessage Out{};
        };

        struct SendStateMachine {
            Future<void> Start(InMessage& msg, TcpConnection* _this) noexcept {
                auto fut = OrderRegulator.GetFuture();
                _this->_Lock.Enter();
                std::swap(_this->Order, fut);
                _this->_Lock.Leave();
                if (!fut.HasState() || fut.IsReady()) {
                    StepSend(_this, &msg);
                }
                else {
                    fut.Then([this, m = &msg, _this](auto&& f) noexcept { StepSend(_this, m); });
                }
                return Complete.GetFuture();
            }
        private:
            void StepSend(TcpConnection* ths, InMessage* msg) noexcept {
                ths->WriteAsync(msg->Length(), msg->GetSend())
                        .Then([this](auto&& f) noexcept { StepComplete(bool(f.Get())); });
            }

            void StepComplete(bool const fail) noexcept {
                OrderRegulator.SetValueUnsafe();
                if (!fail) {
                    Complete.SetValueUnsafe();
                }
                else {
                    Complete.SetExceptionUnsafe(CreateAbortedIo());
                }
                Temp::Delete(this);
            }

            Promise<void> OrderRegulator{};
            Promise<void> Complete{};
        };

        SpinLock _Lock;
        Future<void> Order;
    };

    class TcpHost : public IHostTcp {
    public:
        TcpHost(boost::asio::io_context& ioService, const boost::asio::ip::address& address, const int port)
                :acceptor_(ioService, tcp::endpoint(tcp::v4(), port)), _Context(ioService) { (void) _Srv; }

        Future<std::unique_ptr<IEndPointTcp>> ExpectClient() override {
            Promise<std::unique_ptr<IEndPointTcp>> complete;
            auto fut = complete.GetFuture();
            auto newConn = TcpConnection::Create(_Context);
            auto get = newConn.get();
            acceptor_.async_accept(get->Socket(),
                    [newConn = std::move(newConn), complete = std::move(complete)
                    ](auto&& error) mutable {
                        if (!error) { complete.SetValue(std::move(newConn)); }
                    });
            return fut;
        }

    private:
        IoService _Srv{};
        tcp::acceptor acceptor_;
        boost::asio::io_context& _Context;
    };

    boost::asio::ip::address makeAddress(const std::string_view view) {
        return boost::asio::ip::make_address(std::string(view));
    }
}

std::unique_ptr<IEndPointTcp>
IEndPointTcp::Create(const std::string_view address, const int port) {
    auto ret = std::make_unique<TcpConnection>(IoService::Get());
    ret->Socket().connect({makeAddress(address), static_cast<unsigned short>(port)});
    return ret;
}

std::unique_ptr<IHostTcp> IHostTcp::Create(const std::string_view address, const int port) {
    return std::make_unique<TcpHost>(IoService::Get(), makeAddress(address), port);
}
