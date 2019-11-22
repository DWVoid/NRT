#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include "Msg/Tunnels.h"
#include "Core/Threading/Micro/Promise.h"

void PrintTimePoint(const std::chrono::system_clock::time_point& now, const char* message) {
	std::cout << "    "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
		<< message << std::endl;
}

void AsyncTestComplete(const Future<void>& fut, const std::chrono::system_clock::time_point& now) {
	PrintTimePoint(now, "ms dispatch");
	fut.Wait();
	PrintTimePoint(now, "ms complete");
}

TEST(NrtMsgCorr, TcpCommunicate) {
	auto srv = IHostTcp::Create("127.0.0.1", 31111);
	auto client = IEndPointTcp::Create("127.0.0.1", 31111);
	auto endpoint = srv->ExpectClient().Get();
	char* out = "Hello World";
	endpoint->Send(157, 12, reinterpret_cast<std::byte*>(out)).Wait();
	auto res = client->Wait().Get();
	ASSERT_TRUE(strcmp(out, reinterpret_cast<char*>(res.Data.get())) == 0);
}