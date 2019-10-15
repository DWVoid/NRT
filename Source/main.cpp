#include <iostream>
#include <chrono>
#include "Core/Utilities/TempAlloc.h"
#include "Core/Threading/Micro/Timer.h"

void PrintTimePoint(const std::chrono::system_clock::time_point& now, const char* message) {
	std::cout << "    "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
		<< message << std::endl;
}

void TestSequenceAlloc() {
	puts("\n\nTest - 2'5000'0000 tiny allocations allocator performance test \n");
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

#include "Core/Threading/Micro/Promise.h"

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

void TestThreadMicro() {
    puts("\n\nTest - 1000'0000 raw tasks \n");
    const auto fut = T::Get().GetFuture();
    T test;
    const auto now = std::chrono::system_clock::now();
    for (uint64_t i = 0; i < 1000'0000ll; ++i) {
        ThreadPool::Enqueue(&test);
    }
	AsyncTestComplete(fut, now);
}

#include "Conc/Async.h"

void TestThreadMicroAsync() {
    puts("\n\nTest - 1000'0000 context-less async calls \n");
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

void TestThreadMicro2() {
	puts("\n\nTest - 10'0000 context-less async calls with workload\n");
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

int main() {
    TestSequenceAlloc();
    TestThreadMicro();
    TestThreadMicro2();
    TestThreadMicroAsync();
    puts("\n\nAll Task Completed");
    std::cin.get();
    return 0;
}