#include <limits>
#include "Msg/Tunnels.h"
#if __has_include(<Windows.h>)
#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00
#endif
#include <boost/asio.hpp>
#include <iostream>
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
            boost::system::error_code ec {};
            _Socket.shutdown(boost::asio::socket_base::shutdown_both, ec);
        }
    private:
        IoService _Srv{};
        tcp::socket _Socket;
    };

    class TcpConnection : public IEndPointTcp, TcpIo {
    public:
        explicit TcpConnection(boost::asio::io_service& io_service): TcpIo(io_service) {}

        ~TcpConnection() noexcept override {
            WaitStop().Wait();
            Shutdown();
        }

        static auto Create(boost::asio::io_context& context) { return std::make_unique<TcpConnection>(context); }

        tcp::socket& Socket() noexcept { return Native(); }

        Future<void> Send(InMessage& msg) override {
            auto stm = Temp::New<SendStateMachine>(msg, this);
            return stm->Start();
        }

        Future<OutMessage> Wait() override {
            auto stm = Temp::New<WaitStateMachine>(this);
            return stm->Start();
        }

        Future<void> Stop() override {
            Shutdown();
            return WaitStop();
        }
    private:
        struct SendStateMachine;

        struct StopStruct {
            void Complete() noexcept {
                if (Last) {
                    Temp::Delete(Last);
                }
                Wait.SetValueUnsafe();
                Temp::Delete(this);
            }
            SendStateMachine* Last {nullptr};
            Promise<void> Wait {};
        };

        Future<void> WaitStop() noexcept {
            auto* stop = Temp::New<StopStruct>();
            auto fut = stop->Wait.GetFuture();
            const auto last = TailingState.exchange(reinterpret_cast<SendStateMachine*>(stop)); // NOLINT
            if (last) {
                stop->Last = last;
                const auto v = last->Next.exchange(1);
                if (v == std::numeric_limits<uintptr_t>::max()) { stop->Complete(); }
            }
            else { stop->Complete(); }
            return fut;
        }

        struct WaitStateMachine {
            explicit WaitStateMachine(TcpConnection* _this) noexcept
                    :This(_this) {}

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
                auto fut = Complete.GetFuture();
                MoveNext(0);
                return fut;
            }

            Promise<OutMessage> Complete{};
            TcpMessageHeader Head{};
            TcpConnection* This;
            OutMessage Out{};
        };

        struct SendStateMachine {
            SendStateMachine(InMessage& msg, TcpConnection* ths) noexcept: This(ths), Message(msg) {}

            Future<void> Start() noexcept {
                const auto last = This->TailingState.exchange(this, std::memory_order_relaxed);
                auto fut = Complete.GetFuture();
                if (!last) { StepSend(); }
                else {
                    Last = last;
                    const auto v = last->Next.exchange(reinterpret_cast<uintptr_t>(this), std::memory_order_relaxed);
                    if (v == std::numeric_limits<uintptr_t>::max()) { StepSend(); }
                }
                return fut;
            }
        private:
            void StepSendCon() noexcept {
                Temp::Delete(Last);
                StepSend();
            }

            void StepCompleteCon(bool const fail) noexcept {
                Temp::Delete(Last);
                StepComplete(fail);
            }

            void StepSend() noexcept {
                This->WriteAsync(Message.Length(), Message.GetSend())
                        .Then([this](auto&& f) noexcept { StepComplete(bool(f.Get())); });
            }

            void StepComplete(bool const fail) noexcept {
                auto expect = this;
                auto recover = This->TailingState.compare_exchange_strong(expect, nullptr, std::memory_order_relaxed);
                if (recover) {
                    if (!fail) {
                        Complete.SetValueUnsafe();
                    }
                    else {
                        Complete.SetExceptionUnsafe(CreateAbortedIo());
                    }
                    Temp::Delete(this);
                }
                else {
                    StepCompleteWithCont(fail);
                }
            }

            void StepCompleteWithCont(bool const fail) {
                const auto cont = Next.exchange(std::numeric_limits<uintptr_t>::max(), std::memory_order_relaxed);
                if (!fail) {
                    Complete.SetValueUnsafe();
                    if (cont > 1) { reinterpret_cast<SendStateMachine*>(cont)->StepSendCon(); }
                }
                else {
                    Complete.SetExceptionUnsafe(CreateAbortedIo());
                    if (cont > 1) { reinterpret_cast<SendStateMachine*>(cont)->StepCompleteCon(true); }
                }
                if (cont == 1) {
                    auto stop = reinterpret_cast<StopStruct*>(This->TailingState.load());
                    stop->Complete();
                }
                else {
                    Temp::Delete(this);
                }
            }

            Promise<void> Complete{};
            TcpConnection* This;
            InMessage& Message;
            SendStateMachine* Last { nullptr };
        public:
            std::atomic<uintptr_t> Next { 0 };
        };

        std::atomic<SendStateMachine*> TailingState { nullptr };
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
