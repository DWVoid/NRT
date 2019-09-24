#include <iostream>
#include <chrono>
#include "Utilities/TempAlloc.h"
#include "Threading/Micro/Timer.h"

void TestSequenceAlloc() {
	puts("\n\nTest - 2'5000'0000 tiny allocations allocator performance test \n");
	const auto array = new uintptr_t[2'5000'0000ll];
	{
		const auto now = std::chrono::system_clock::now();
		for (volatile int64_t i = 0; i < 2'5000'0000ll; ++i) {}
		std::cout << "    "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
			<< "ms spin" << std::endl;
	}
	{
		const auto now = std::chrono::system_clock::now();
		for (uint64_t i = 0; i < 2'5000'0000ll; ++i) {
			array[i] = 0;
		}
		std::cout << "    "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
			<< "ms assign heat-up" << std::endl;
	}
	{
		const auto now = std::chrono::system_clock::now();
		for (uint64_t i = 0; i < 2'5000'0000ll; ++i) {
			array[i] = 1;
		}
		std::cout << "    "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
			<< "ms assign" << std::endl;
	}

	{
		const auto now = std::chrono::system_clock::now();
		for (uint64_t i = 0; i < 2'5000'0000ll; ++i) {
			array[i] = reinterpret_cast<uintptr_t>(Temp::Allocate(8));
		}
		std::cout << "    "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
			<< "ms alloc" << std::endl;
	}
	{
		const auto now = std::chrono::system_clock::now();
		for (uint64_t i = 0; i < 2'5000'0000ll; ++i) {
			Temp::Deallocate(reinterpret_cast<void*>(array[i]), 8);
		}
		std::cout << "    "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
			<< "ms free" << std::endl;
	}
	{
		const auto now = std::chrono::system_clock::now();
		Temp::Collect();
		std::cout << "    "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
			<< "ms collect" << std::endl;
	}
	delete[] array;
}

#include "Threading/Micro/Promise.h"

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

void TestThreadMicro() {
	puts("\n\nTest - 1000'0000 raw tasks \n");
	auto fut = T::Get().GetFuture();
	T test;
	const auto now = std::chrono::system_clock::now();
	for (uint64_t i = 0; i < 1000'0000ll; ++i) {
		ThreadPool::Enqueue(&test);
	}
	std::cout << "    "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
		<< "ms dispatch" << std::endl;
	fut.Wait();
	std::cout << "    "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
		<< "ms complete" << std::endl;
	//3.84M/s
	//1000 clock/task
}

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
				puts("    done");
				current.SetValueUnsafe();
			}
			});
	}
	fut.Wait();
	std::cout << "    "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count()
		<< "ms dispatch" << std::endl;
}

void TestThreadMicro2() {
    {
        srand(0);
        Promise<void> current;
        const auto fut = current.GetFuture();
        const auto now = std::chrono::system_clock::now();
        std::atomic_int cnt = 0;
        for (uint64_t i = 0; i<10'0000ll; ++i) {
            AsyncLight([&]()noexcept {
	            const int c = (static_cast<double>(rand()) / double(RAND_MAX) * 100'0000.0);
                for (volatile int i = 0; i < c; ++i) {
                }
	            const auto val = cnt.fetch_add(1);
                if (val==9'9999ll) {
                    puts("    done");
                    current.SetValueUnsafe();
                }
            });
        }
        puts("done submit");
        fut.Wait();
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-now).count()
                  << "ms dispatch" << std::endl;
    }
}

int main() {
	TestSequenceAlloc();
	TestThreadMicro();
    //TestThreadMicro2();
    TestThreadMicroAsync();
	puts("\n\nAll Task Completed");
	std::cin.get();
    return 0;
}