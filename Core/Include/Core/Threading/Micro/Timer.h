#pragma once

#include <atomic>
#include <chrono>
#include "ThreadPool.h"

class DelayedTask : public virtual IExecTask {
public:
    template <class Rep, class Period>
    explicit DelayedTask(const std::chrono::duration<Rep, Period>& duration) noexcept
        : DelayedTask(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) {}
    NRTCORE_API void Submit() noexcept;
	NRTCORE_API void Cancel() noexcept;
    virtual void OnCancelled() noexcept {}
	[[nodiscard]] bool IsCancelled() const noexcept { return _Cancel; }
private:
    explicit DelayedTask(const unsigned long long milliseconds) noexcept: _Milli(milliseconds) {}
    unsigned long long _Milli;
    bool _Cancel = false;
};

class CycleTask : public DelayedTask {
public:
    template <class Rep, class Period>
    explicit CycleTask(const std::chrono::duration<Rep, Period>& duration) noexcept: DelayedTask(duration) {}
    virtual void OnTimer() noexcept = 0;
    void Enable() noexcept { if (!_Enabled.exchange(true)) { Submit(); } }
    void Disable() noexcept { _Enabled.store(false); Cancel(); }
private:
	NRTCORE_API void Exec() noexcept final override;
    std::atomic_bool _Enabled {false};
};
