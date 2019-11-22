#include "Msg/Tunnels.h"
#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00
#include <boost/asio.hpp>
#include "Core/Threading/SpinLock.h"

using boost::asio::ip::tcp;

namespace {
	struct IoService {
		inline static boost::asio::io_context Context{};
		inline static std::atomic_int Counter{0};
		using Guard = boost::asio::executor_work_guard<decltype(Context.get_executor())>;
		inline static std::aligned_storage_t<sizeof(Guard), alignof(Guard)> Stg;

		IoService() noexcept {
			if (Counter.fetch_add(1) == 0) {
				(void)new(&Stg)Guard(Context.get_executor());
				std::thread([]() { Context.run(); }).detach();
			}
		}

		~IoService() noexcept {
			if (Counter.fetch_sub(1) == 1) {
				const auto stg = reinterpret_cast<Guard*>(&Stg);
				stg->reset();
				stg->~Guard();
			}
		}

		static auto& Get() noexcept { return Context; }
	};

	struct TcpMessageHeader {
		TcpMessageHeader() noexcept = default;

		TcpMessageHeader(uint32_t type, uint32_t length) noexcept
			: Data{
				static_cast<std::byte>(type >> 24), static_cast<std::byte>(type >> 16),
				static_cast<std::byte>(type >> 8), static_cast<std::byte>(type),
				static_cast<std::byte>(length >> 24), static_cast<std::byte>(length >> 16),
				static_cast<std::byte>(length >> 8), static_cast<std::byte>(length),
			} {}

		[[nodiscard]] uint32_t Type() const noexcept {
			return uint32_t(Data[0]) << 24 | uint32_t(Data[1]) << 16 | uint32_t(Data[2]) << 8 | uint32_t(Data[3]);
		}

		[[nodiscard]] uint32_t Length() const noexcept {
			return uint32_t(Data[4]) << 24 | uint32_t(Data[5]) << 16 | uint32_t(Data[6]) << 8 | uint32_t(Data[7]);
		}

		std::byte Data[8];
	};

	auto CreateAbortedIo() noexcept { return std::make_exception_ptr(AbortedIo{}); }

	class TcpConnection : public IEndPointTcp {
	public:
		explicit TcpConnection(boost::asio::io_service& io_service)
			: _Socket(io_service) { (void)_Srv; }

		static auto Create(boost::asio::io_context& context) { return std::make_unique<TcpConnection>(context); }

		tcp::socket& Socket() noexcept { return _Socket; }

		Future<void> Send(uint32_t type, uint32_t length, std::byte* content) override {
			auto stm = Temp::New<SendStateMachine>(type, length, content, this);
			return stm->Start();
		}

		Future<OutMessage> Wait() override {
			auto stm = Temp::New<WaitStateMachine>(this);
			return stm->Start();
		}

		Future<void> Stop() override {
			_Socket.shutdown(boost::asio::socket_base::shutdown_both);
			_Lock.Enter();
			auto last = std::move(Order);
			_Lock.Leave();
			return last;
		}

	private:
		struct WaitStateMachine {
			explicit WaitStateMachine(TcpConnection* _this) noexcept
				: This(_this) {}

			void MoveNext(const uint32_t step) noexcept {
				switch (step) {
				case 0:
					This->ReadAsync(8, Head.Data).Then([this](auto&& f) noexcept { MoveNext((f.Get() ? -1 : 1)); });
					return;
				case 1:
					Out.Type = Head.Type();
					Out.Length = Head.Length();
					Out.Data = decltype(Out.Data)(Temp::Allocator<std::byte>().allocate(Head.Length()),
					                              {Head.Length()});
					This->ReadAsync(Head.Length(), Out.Data.get()).Then([this](auto&& f) noexcept {
						MoveNext((f.Get() ? -1 : 2));
					});
					return;
				case 2:
					Complete.SetValueUnsafe(std::move(Out));
					break;
				default:
					Complete.SetExceptionUnsafe(CreateAbortedIo());
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
			SendStateMachine(const uint32_t type, const uint32_t length, std::byte* content,
			                 TcpConnection* _this) noexcept
				: Head(type, length), This(_this), Content(content), Length(length) {}

			void MoveNext(const uint32_t step) noexcept {
				switch (step) {
				case 0: {
					This->_Lock.Enter();
					auto last = std::move(This->Order);
					This->Order = OrderRegulator.GetFuture();
					This->_Lock.Leave();
					if (last.HasState()) {
						last.Then([this](auto&& f) noexcept { MoveNext(1); });
						return;
					}
				}
				case 1:
					This->WriteAsync(8, Head.Data).Then([this](auto&& f) noexcept { MoveNext((f.Get() ? -1 : 2)); });
					return;
				case 2:
					This->WriteAsync(Length, Content).Then([this](auto&& f) noexcept { MoveNext((f.Get() ? -1 : 3)); });
					return;
				case 3:
					Complete.SetValueUnsafe();
					break;
				default:
					Complete.SetExceptionUnsafe(CreateAbortedIo());
				}
				OrderRegulator.SetValueUnsafe();
				Temp::Delete(this);
			}

			Future<void> Start() noexcept {
				MoveNext(0);
				return Complete.GetFuture();
			}

			Promise<void> OrderRegulator{};
			Promise<void> Complete{};
			TcpMessageHeader Head;
			TcpConnection* This;
			std::byte* Content;
			uint32_t Length;
		};

		Future<boost::system::error_code> WriteAsync(uint32_t length, std::byte* content) noexcept {
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

		IoService _Srv{};
		tcp::socket _Socket;
		SpinLock _Lock;
		Future<void> Order;
	};

	class TcpHost : public IHostTcp {
	public:
		TcpHost(boost::asio::io_context& ioService, const boost::asio::ip::address& address, const int port)
			: acceptor_(ioService, tcp::endpoint(tcp::v4(), port)), _Context(ioService) { (void)_Srv; }

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

std::unique_ptr<IEndPointTcp> IEndPointTcp::Create(const std::string_view address, const int port) {
	auto ret = std::make_unique<TcpConnection>(IoService::Get());
	ret->Socket().connect({makeAddress(address), static_cast<unsigned short>(port)});
	return ret;
}

std::unique_ptr<IHostTcp> IHostTcp::Create(const std::string_view address, const int port) {
	return std::make_unique<TcpHost>(IoService::Get(), makeAddress(address), port);
}
