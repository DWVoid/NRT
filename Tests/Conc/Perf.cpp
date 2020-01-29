#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include "Conc/Async.h"
#include "Cfx/Threading/Micro/Timer.h"

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

TEST(NrtConcPref, ThreadMicroAsync) {
	Promise<void> current;
	const auto fut = current.GetFuture();
	const auto now = std::chrono::system_clock::now();
	std::atomic_int cnt = 0;
	for (uint64_t i = 0; i < 1000'0000ll; ++i) {
		AsyncLight([&]()noexcept {
			const auto val = cnt.fetch_add(1);
			if (val == 999'9999ll) {
				current.SetValueUnsafe();
			}
			});
	}
	AsyncTestComplete(fut, now);
}

TEST(NrtConcPref, ThreadMicro2Loaded) {
	srand(0);
	Promise<void> current;
	const auto fut = current.GetFuture();
	const auto now = std::chrono::system_clock::now();
	std::atomic_int cnt = 0;
	for (uint64_t i = 0; i < 10'0000ll; ++i) {
		AsyncLight([&]()noexcept {
			const int c = (static_cast<double>(rand()) / double(RAND_MAX) * 100'0000.0);
			for (volatile int i = 0; i < c; ++i) {
			}
			const auto val = cnt.fetch_add(1);
			if (val == 9'9999ll) {
				current.SetValueUnsafe();
			}
			});
	}
	AsyncTestComplete(fut, now);
}
