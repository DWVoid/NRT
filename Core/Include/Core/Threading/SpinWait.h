#pragma once

#include <thread>
#include <limits>
#include "Core/Config.h"

#if __has_include(<x86intrin.h>)
#include <x86intrin.h>
#define IDLE _mm_pause()
#elif __has_include(<intrin.h>)
#include <intrin.h>
#define IDLE _mm_pause()
#else
#define IDLE asm("nop")
#endif

struct SpinWait {
private:
    static constexpr unsigned int YieldThreshold = 10; // When to switch over to a true yield.
    static constexpr unsigned int Sleep0EveryHowManyYields = 5; // After how many yields should we Sleep(0)?
    static constexpr unsigned int DefaultSleep1Threshold = 20; // After how many yields should we Sleep(1) frequently?
	NRTCORE_API static bool IsSingleProcessor;
public:
	NRTCORE_API static unsigned int OptimalMaxSpinWaitsPerSpinIteration;
	NRTCORE_API static unsigned int SpinCountForSpinBeforeWait;

    [[nodiscard]] unsigned int Count() const noexcept { return _Count; }

    [[nodiscard]] bool NextSpinWillYield() const noexcept { return _Count>=YieldThreshold || IsSingleProcessor; }

    void SpinOnce() noexcept {
        SpinOnceCore(DefaultSleep1Threshold);
    }

    void SpinOnce(unsigned int sleep1Threshold) noexcept {
        if (sleep1Threshold>=0 && sleep1Threshold<YieldThreshold) {
            sleep1Threshold = YieldThreshold;
        }
        SpinOnceCore(sleep1Threshold);
    }

    void Reset() noexcept {
        _Count = 0;
    }

private:
    unsigned int _Count = 0;

    void SpinOnceCore(const unsigned int sleep1Threshold) noexcept {
        if ((_Count>=YieldThreshold
                && ((_Count>=sleep1Threshold && sleep1Threshold>=0) || (_Count-YieldThreshold)%2==0))
                || IsSingleProcessor) {
            if (_Count>=sleep1Threshold && sleep1Threshold>=0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else {
                const int yieldsSoFar = _Count>=YieldThreshold ? (_Count-YieldThreshold)/2 : _Count;
                if ((yieldsSoFar%Sleep0EveryHowManyYields)==(Sleep0EveryHowManyYields-1)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(0));
                }
                else {
                    std::this_thread::yield();
                }
            }
        }
        else {
	        auto n = OptimalMaxSpinWaitsPerSpinIteration;
            if (_Count<=30 && (1u << _Count)<n) {
                n = 1u << _Count;
            }
            Spin(n);
        }

        // Finally, increment our spin counter.
        _Count = (_Count==std::numeric_limits<int>::max() ? YieldThreshold : _Count + 1);
    }

    static void Spin(const unsigned int iterations) noexcept {
        for (auto i = 0u; i<iterations; ++i) {
            IDLE;
        }
    }
};