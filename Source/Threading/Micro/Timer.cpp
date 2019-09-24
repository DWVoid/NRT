#include "Utilities/InterOp.inc"
#include "Timer.h"
#include <thread>
#include <queue>
#include <mutex>

namespace {
	class TimeAgent {
	public:
		~TimeAgent() { Utilities::InterOp::StopOnTimerStop(); }

		void Stop() {
			if (!_Stop.exchange(true)) {
				_QueueNotify.notify_all();
				if (_Agent.joinable()) {
					_Agent.join();
				}
			}
		}

		void Submit(DelayedTask* task, int milli) noexcept {
			std::lock_guard<std::mutex> lk(_QueueLock);
			_Queue.push({ task, std::chrono::steady_clock::now() + std::chrono::milliseconds(milli) });
			_QueueNotify.notify_all();
		}

		bool Running() const noexcept { return !_Stop; }

		void Cancel() { _QueueNotify.notify_all(); }
	private:
		struct DelayEntry {
			DelayedTask* Task;
			std::chrono::steady_clock::time_point Due;
		};

		struct Comp { 
			constexpr bool operator()(const DelayEntry& l, const DelayEntry& r) const noexcept {
				return l.Due > r.Due;
			}
		};
		
		void Worker() noexcept {
			std::unique_lock<std::mutex> lk{ _QueueLock };
			while (!_Stop) {
				if (!_Queue.empty()) {
					while (std::chrono::steady_clock::now() < _Queue.top().Due && !_Queue.top().Task->IsCancelled()) {
						_QueueNotify.wait_until(lk, _Queue.top().Due);
					}
					if (auto task = _Queue.top().Task; task->IsCancelled()) {
						task->OnCancelled();
					}
					else {
						ThreadPool::Enqueue(task);
					}
					_Queue.pop();
				}
				else {
					_QueueNotify.wait(lk);
				}
			}
		}
		std::atomic_bool _Stop {false};
		std::mutex _QueueLock;
		std::condition_variable _QueueNotify;
		std::priority_queue<DelayEntry, std::vector<DelayEntry>, Comp> _Queue;
		std::thread _Agent = std::thread([this]() noexcept { Worker(); });
	} _Agent;
}

void DelayedTask::Submit() noexcept { _Cancel = false; _Agent.Submit(this, _Milli); }

void DelayedTask::Cancel() noexcept { _Cancel = true; _Agent.Cancel(); }

void CycleTask::Exec() noexcept {
	if (_Enabled && _Agent.Running()) {
		OnTimer();
		Submit();
	}
}

namespace Utilities::InterOp {
	void StopOnTimerStop() noexcept {
		StopOnEdenCollectorStop();
		_Agent.Stop();
	}
}
