#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include <Service.h>
#include "Msg/Tunnels.h"
#include "Cfx/Threading/Micro/Promise.h"

void PrintTimePoint(const std::chrono::system_clock::time_point& now, const char* message) {
	std::cout << "    "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
		<< message << std::endl;
}

void Hint(const char* ch, int i) {
    if (i % 10000 == 0) {
        std::cout << ch << i << std::endl;
    }
}

TEST(NrtMsgCorr, TcpCommunicate) {
    NEWorld::ServiceHandle x { "org.newinfinideas.nrt.cfx.temp_alloc" };
	auto srv = IHostTcp::Create("127.0.0.1", 31111);
	InMessagePersistent message(157, 12);
	memcpy(message.Get(), "Hello World", 12);
    auto z = std::thread([&]() {
        auto client = IEndPointTcp::Create("127.0.0.1", 31111);
        for (int i = 0; i < 1000000; ++i) {
            auto res = client->Wait().Get();
            //Hint("r:", i);
        }
    });
	auto y = std::thread([&]() {
        auto endpoint = srv->ExpectClient().Get();
        for (int i = 0; i<1000000; ++i) {
            endpoint->Send(message).Wait();
            //Hint("s:", i);
        }
    });
    z.join();
    y.join();
}