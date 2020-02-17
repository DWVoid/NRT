#include <cstddef>
#include <vector>
#include <future>
#include "Service.h"
#include "Cfx/Threading/Micro/ThreadPool.h"
#include "Cfx/Threading/SpinLock.h"
#include "Cfx/Threading/Semaphore.h"
#include "Cfx/Utilities/TempAlloc.h"

namespace {
    class SyncQueue {
    public:
        void Push(const uintptr_t data) noexcept {
            if (!_Beg) {
                _Beg.Off = _End.Off = 0;
                _Beg.Blk = _End.Blk = Temp::New<Node>();
                _End.Blk->Data[_End.Off++] = data;
            }
            else {
                _End.Blk->Data[_End.Off++] = data;
                if (_End.Off == Items) {
                    _End.Off = 0;
                    _End.Blk->Next = Temp::New<Node>();
                    _End.Blk = _End.Blk->Next;
                }
            }
        }

        uintptr_t Pop() noexcept {
            if (_Beg) {
                const auto ret = _Beg.Blk->Data[_Beg.Off++];
                if (_Beg != _End) {
                    if (_Beg.Off == Items) {
                        _Beg.Off = 0;
                        const auto rel = _Beg.Blk;
                        _Beg.Blk = _Beg.Blk->Next;
                        Temp::Delete(rel);
                    }
                }
                if (_Beg == _End) {
                    if (_Beg.Off == _End.Off) {
                        const auto rel = _Beg.Blk;
                        _Beg.Blk = _End.Blk = nullptr;
                        Temp::Delete(rel);
                    }
                }
                return ret;
            }
            return 0;
        }

        [[nodiscard]] bool Empty() const noexcept { return !_Beg.Blk; }
    private:
        static constexpr uintptr_t Items = 1024 / sizeof(uintptr_t) - 1;

        struct Node {
            constexpr Node() noexcept : Next(nullptr), Data() {}
            Node* Next;
            uintptr_t Data[Items];
        };

        struct Iterator {
            constexpr Iterator() noexcept : Blk(nullptr), Off(0) {}
            explicit operator bool() const noexcept { return Blk; }
            bool operator !=(const Iterator& r) const noexcept { return (Blk != r.Blk) || (Off != r.Off); }
            bool operator ==(const Iterator& r) const noexcept { return (Blk == r.Blk) && (Off == r.Off); }
            Node* Blk;
            uintptr_t Off;
        };

        Iterator _Beg{}, _End{};
    };

    class Queue {
    public:
        Queue* Next = nullptr;

        void Push(IExecTask* task) noexcept {
            _Spin.Enter();
            _Exec.Push(reinterpret_cast<uintptr_t>(task));
            _Spin.Leave();
        }

        IExecTask* Pop() noexcept {
            _Spin.Enter();
            const auto ret = reinterpret_cast<IExecTask*>(_Exec.Pop());
            _Spin.Leave();
            return ret;
        }

        [[nodiscard]] bool SnapshotCheck() const noexcept {
            if (!_Exec.Empty()) { return true; }
            for (auto current = Next; current != this; current = current->Next) {
                if (!current->_Exec.Empty()) { return true; }
            }
            return false;
        }

        IExecTask* TryDequeue() noexcept {
            if (const auto res = Pop(); res) { return res; }
            for (auto current = Next; current != this; current = current->Next) {
                if (!current->_Exec.Empty()) { if (const auto res = current->Pop(); res) { return res; } }
            }
            return nullptr;
        }

    private:
        SpinLock _Spin {};
        SyncQueue _Exec {};
    };

    class QueueGroup {
    public:
        void Add(Queue* queue) noexcept {
            _Lock.Enter();
            if (_First) {
                queue->Next = _First->Next;
                _First->Next = queue;
            }
            else {
                _First = queue;
                _First->Next = _First;
            }
            _Lock.Leave();
        }

        void Clear() noexcept {
            _Lock.Enter();
            for (auto current = _First->Next; current != _First;) {
                const auto op = current->Next;
                delete current;
                current = op;
            }
            delete _First;
            _First = nullptr;
            _Lock.Leave();
        }

        static auto& Instance() {
            static QueueGroup instance{};
            return instance;
        }

    private:
        SpinLock _Lock;
        Queue* _First{nullptr};
    };

    auto& GetCurrentQueue() {
        static thread_local Queue* queue;
        return queue;
    }

    thread_local int _InstanceInvokeId;

    class Implementation: public NEWorld::Object {
    public:
        Implementation() {
            const auto threadCount = static_cast<int>(std::thread::hardware_concurrency() - 1);
            QueueGroup::Instance().Add(_GlobalQueue);
            _Workers.reserve(threadCount);
            for (auto i = 0; i < threadCount; i++) { _Workers.emplace_back(std::make_unique<Worker>(*this)); }
        }

        ~Implementation() noexcept override { Stop(); }

        void Stop() noexcept {
            if (_Stat.Running.exchange(false)) {
                WakeAll();
                _Workers.clear();
                _Workers.shrink_to_fit();
            }
            QueueGroup::Instance().Clear();
        }

        void WakeOne() noexcept {
            for (;;) {
                if (auto c = _Park.Count.load(); c) {
                    if (_Park.Count.compare_exchange_strong(c, c - 1)) {
                        _Park.Signal.Signal();
                        return;
                    }
                }
                else { return; }
            }
        }

        void WakeAll() noexcept {
            const auto c = _Park.Count.exchange(0);
            for (auto i = 0; i < c; ++i) { _Park.Signal.Signal(); }
        }

        void MakePanic() noexcept { _Stat.Panic = true; }

        Queue& Global() const noexcept { return *_GlobalQueue; }

        [[nodiscard]] int Count() const noexcept { return _Workers.size(); }
    private:
        struct Panic final : std::exception {};

        struct RuntimeInfo {
            std::atomic_int MaxId = 0;
            std::atomic_bool Running{true};
            std::atomic_bool Panic{false};
        } _Stat;

        struct Park {
            std::atomic_int Count{0};
            Semaphore Signal{};
        } _Park;

        class Worker {
        public:
            explicit Worker(Implementation& pool) noexcept : _Pool(pool) {
                _Thread = std::thread([this]() {
                    Init();
                    Work();
                });
            }

            Worker(Worker&&) noexcept = default;

            Worker& operator=(Worker&&) noexcept = delete;

            Worker(const Worker&) = delete;

            Worker& operator=(const Worker&) = delete;

            ~Worker() { if (_Thread.joinable()) { _Thread.join(); } }

            void Init() noexcept {
                InitQueue();
                InitInvokeId();
            }

            void Work() const noexcept {
                try {
                    while (_Pool._Stat.Running) {
                        DoWorks();
                        if (_Pool._Stat.Running) { Rest(); }
                    }
                }
                catch (...) { PanicIfNotAlready(); }
            }

        private:
            void InitQueue() const noexcept {
                auto& queue = GetCurrentQueue();
                queue = _Queue;
                QueueGroup::Instance().Add(queue);
            }

            void InitInvokeId() const noexcept { _InstanceInvokeId = _Pool._Stat.MaxId.fetch_add(1); }

            [[nodiscard]] IExecTask* FetchWork() const {
                if (_Pool._Stat.Panic) { WorkerPanic(); }
                if (auto exec = _Queue->TryDequeue(); exec) { return exec; }
                else {
                    SpinWait spinner{};
                    for (auto i = 0u; i < SpinWait::SpinCountForSpinBeforeWait; ++i) {
                        spinner.SpinOnce();
                        if (exec = _Queue->TryDequeue(); exec) { return exec; }
                    }
                    return nullptr;
                }
            }

            void Rest() const {
                _Pool._Park.Count.fetch_add(1); // enter protected region
                if (_Queue->SnapshotCheck()) {
                    // it is possible that a task was added during function invocation period of this function and the WakeOne
                    // did not notice this thread is going to sleep. To prevent system stalling, we will invoke WakeOne again
                    // to woke a worker (including this one)
                    _Pool.WakeOne();
                }
                // to keep integrity, this thread will enter sleep state regardless of whether if the snapshot check is positive
                _Pool._Park.Signal.Wait();
                if (_Pool._Stat.Panic) { WorkerPanic(); }
            }

            void PanicIfNotAlready() const { if (!_Pool._Stat.Panic.exchange(true)) { ThreadPool::Panic(); } }

            void DoWorks() const {
                for (;;) {
                    if (auto exec = FetchWork(); exec) {
                        exec->Exec();
                    }
                    else { return; }
                }
            }

            [[noreturn]] static void WorkerPanic() { throw Panic{}; }

            std::thread _Thread;
            Implementation& _Pool;
            Queue* _Queue = new Queue();
        };

        NEWorld::ServiceHandle _Alloc { "org.newinfinideas.nrt.cfx.temp_alloc.a" };
        Queue* _GlobalQueue = new Queue();
        std::vector<std::unique_ptr<Worker>> _Workers;
    };

    NW_MAKE_SERVICE(Implementation, "org.newinfinideas.nrt.cfx.thread_pool", 0.0, _Impl)
}

bool ThreadPool::LocalEnqueue(IExecTask* task) noexcept {
    if (const auto queue = GetCurrentQueue(); queue) {
        queue->Push(task);
        _Impl.Get().WakeOne();
        return true;
    }
    return false;
}

void ThreadPool::Enqueue(IExecTask* task) noexcept {
    if (!LocalEnqueue(task)) {
        _Impl.Get().Global().Push(task);
        _Impl.Get().WakeOne();
    }
}

void ThreadPool::Spawn(AInstancedExecTask* task) noexcept {
    class Warp: public IExecTask {
    public:
        void Exec() noexcept override {
            Task->Exec(_InstanceInvokeId);
            if (Completes.fetch_add(1) + 1 == Threads) {
                Task->OnComplete();
                Temp::Delete(this);
            }
        }

        static auto Create(AInstancedExecTask* const task) noexcept {
            const auto w = Temp::New<Warp>();
            w->Task = task;
            return w;
        }
    private:
        AInstancedExecTask* Task {};
        const int Threads = _Impl.Get().Count();
        std::atomic_int Completes { 0 };
    };

    const auto zero = &_Impl.Get().Global();
    const auto w = Warp::Create(task);
    for (auto current = zero->Next; current != zero; current = current->Next) { current->Push(w); }
    _Impl.Get().WakeAll();
}

void ThreadPool::Panic() noexcept {
    _Impl.Get().MakePanic();
    _Impl.Get().Stop();
}

int ThreadPool::CountThreads() noexcept { return _Impl.Get().Count(); }
