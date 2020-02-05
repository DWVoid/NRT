#pragma once

#include "Config.h"
#include "Compile/Endian.h"
#include "Cfx/Threading/Micro/Promise.h"

struct AbortedIo : std::exception {};

struct InMessage {
    [[nodiscard]] auto Get() const noexcept { return Buffer + 8; }
    [[nodiscard]] auto GetSend() const noexcept { return Buffer; }
    [[nodiscard]] auto Length() const noexcept { return le32toh(*reinterpret_cast<uint32_t*>(Buffer + 4)) + 8; }
protected:
    explicit InMessage(void* const buffer) noexcept: Buffer(static_cast<std::byte* const>(buffer)) {}
    std::byte* const Buffer;
};

struct InMessageBuffer: InMessage {
    InMessageBuffer(uint32_t type, uint32_t length) noexcept: InMessage(Temp::Allocate(length + 8u)) {
        *reinterpret_cast<uint32_t*>(Buffer) = htole32(type);
        *(reinterpret_cast<uint32_t*>(Buffer + 4)) = htole32(length);
    }
    ~InMessageBuffer() noexcept {
        Temp::Deallocate(Buffer, Length());
    }
};

struct InMessagePersistent: InMessage {
    InMessagePersistent(uint32_t type, uint32_t length) noexcept: InMessage(malloc(length + 8u)) {
        *reinterpret_cast<uint32_t*>(Buffer) = htole32(type);
        *(reinterpret_cast<uint32_t*>(Buffer + 4)) = htole32(length);
    }
    ~InMessagePersistent() noexcept { free(Buffer); }
};

struct OutMessage {
	struct Release {
		uint32_t Length;
		void operator()(void* ptr) const noexcept { Temp::Deallocate(ptr, Length); }
	};
	uint32_t Type;
	uint32_t Length;
	std::unique_ptr<std::byte[], Release> Data;
};

struct IEndPointTcp {
	IEndPointTcp() noexcept = default;
	IEndPointTcp(IEndPointTcp&&) = delete;
	IEndPointTcp(const IEndPointTcp&) = delete;
	IEndPointTcp& operator=(IEndPointTcp&&) = delete;
	IEndPointTcp& operator=(const IEndPointTcp&) = delete;
	virtual ~IEndPointTcp() noexcept = default;
	virtual Future<void> Send(InMessage& msg) = 0;
	virtual Future<OutMessage> Wait() = 0;
	virtual Future<void> Stop() = 0;
	NRTMSG_API static std::unique_ptr<IEndPointTcp> Create(std::string_view address, int port);
};

struct IHostTcp {
	IHostTcp() noexcept = default;
	IHostTcp(IHostTcp&&) = delete;
	IHostTcp(const IHostTcp&) = delete;
	IHostTcp& operator=(IHostTcp&&) = delete;
	IHostTcp& operator=(const IHostTcp&) = delete;
	virtual ~IHostTcp() noexcept = default;
	virtual Future<std::unique_ptr<IEndPointTcp>> ExpectClient() = 0;
	NRTMSG_API static std::unique_ptr<IHostTcp> Create(std::string_view address, int port);
};