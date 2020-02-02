#pragma once

#include "Config.h"
#include "Cfx/Threading/Micro/Promise.h"

struct AbortedIo : std::exception {
	
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
	virtual Future<void> Send(uint32_t type, uint32_t length, const std::byte* content) = 0;
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