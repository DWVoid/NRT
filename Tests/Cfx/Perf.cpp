#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include "Cfx/Utilities/TempAlloc.h"
#include "Cfx/Threading/Micro/Timer.h"

void PrintTimePoint(const std::chrono::system_clock::time_point& now, const char* message) {
	std::cout << "    "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
		<< message << std::endl;
}

 TEST(NrtCorePerf, SequenceAlloc250000000) {
	const auto array = new uintptr_t[2'5000'0000ll];
	{
		const auto now = std::chrono::system_clock::now();
		for (volatile int64_t i = 0; i < 2'5000'0000ll; ++i) {}
		PrintTimePoint(now, "ms spin");
	}
	{
		const auto now = std::chrono::system_clock::now();
		for (uint64_t i = 0; i < 2'5000'0000ll; ++i) {
			array[i] = 0;
		}
		PrintTimePoint(now, "ms assign heat-up");
	}
	{
		const auto now = std::chrono::system_clock::now();
		for (uint64_t i = 0; i < 2'5000'0000ll; ++i) {
			array[i] = 1;
		}
		PrintTimePoint(now, "ms assign");
	}

	{
		const auto now = std::chrono::system_clock::now();
		for (uint64_t i = 0; i < 2'5000'0000ll; ++i) {
			array[i] = reinterpret_cast<uintptr_t>(Temp::Allocate(8));
		}
		PrintTimePoint(now, "ms alloc");
	}
	{
		const auto now = std::chrono::system_clock::now();
		for (uint64_t i = 0; i < 2'5000'0000ll; ++i) {
			Temp::Deallocate(reinterpret_cast<void*>(array[i]), 8);
		}
		PrintTimePoint(now, "ms free");
	}
	{
		const auto now = std::chrono::system_clock::now();
		Temp::Collect();
		PrintTimePoint(now, "ms collect");
	}
	delete[] array;
}

#include "Cfx/Threading/Micro/Promise.h"

class T : public IExecTask {
public:
	static auto& Get() {
		static auto current = new Promise<void>();
		return *current;
	}

	inline static std::atomic_int cnt = 0;

	void Exec() noexcept override {
		if (cnt.fetch_add(1) == 999'9999ll) {
			Get().SetValueUnsafe();
			delete& Get();
		}
	}
};

void AsyncTestComplete(const Future<void>& fut, const std::chrono::system_clock::time_point& now) {
	PrintTimePoint(now, "ms dispatch");
	fut.Wait();
	PrintTimePoint(now, "ms complete");
}

TEST(NrtCorePerf, ThreadMicro10000000) {
	const auto fut = T::Get().GetFuture();
	T test;
	const auto now = std::chrono::system_clock::now();
	for (uint64_t i = 0; i < 1000'0000ll; ++i) {
		ThreadPool::Enqueue(&test);
	}
	AsyncTestComplete(fut, now);
}
