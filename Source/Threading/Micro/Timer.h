#pragma once

#include <atomic>
#include <chrono>
#include "ThreadPool.h"

class DelayedTask : public virtual IExecTask {
public:
	template <class Rep, class Period>
	explicit DelayedTask(const std::chrono::duration<Rep, Period>& duration) noexcept
		: DelayedTask(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) {}
	void Submit() noexcept;
	void Cancel() noexcept;
	bool IsCancelled() noexcept { return _Cancel; }
	virtual void OnCancelled() noexcept {}
private:
	explicit DelayedTask(int milliseconds) noexcept: _Milli(milliseconds) {}
	int _Milli;
	bool _Cancel = false;
};

class CycleTask : public DelayedTask {
public:
	template <class Rep, class Period>
	explicit CycleTask(const std::chrono::duration<Rep, Period>& duration) noexcept: DelayedTask(duration) {}
	virtual void OnTimer() noexcept = 0;
	void Exec() noexcept override;
	void Enable() noexcept { if (!_Enabled.exchange(true)) { Submit(); } }
	void Disable() noexcept { _Enabled.store(false); Cancel(); }
private:
	std::atomic_bool _Enabled {false};
};
