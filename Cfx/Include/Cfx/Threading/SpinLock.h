#pragma once

#include <atomic>
#include "Lock.h"
#include "SpinWait.h"

class SpinLock {
public:
    void Enter() noexcept {
        for (;;) {
            auto expect = false;
            if (__lock.compare_exchange_strong(expect, true, std::memory_order_acquire))
                return;
            WaitUntilLockIsFree();
        }
    }

    void Leave() noexcept { __lock.store(false, std::memory_order_release); }
private:
    void WaitUntilLockIsFree() const noexcept {
        SpinWait spinner{};
        while (__lock.load(std::memory_order_relaxed)) { spinner.SpinOnce(); }
    }

    alignas(64) std::atomic_bool __lock = {false};
};
