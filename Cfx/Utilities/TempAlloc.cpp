#include <cstddef>
#include "Service.h"
#include "System/Memory.h"
#include "Cfx/Utilities/TempAlloc.h"
#include "Cfx/Threading/SpinLock.h"
#include "Cfx/Threading/Micro/Timer.h"
#include "Cfx/Threading/Micro/ThreadPool.h"

namespace {
    constexpr uintptr_t G0VacateThreshold = 5;
    constexpr uintptr_t G1VacateThresholdThread = 2;

    struct Block final {
        int32_t Alloc = 0;
        Block* Next = nullptr;
        alignas(NEWorld::System::Memory::Amd64CpuCacheSize) std::atomic_int32_t Dealloc{0};
    };

    static_assert(offsetof(Block, Dealloc)==NEWorld::System::Memory::Amd64CpuCacheSize, "Incorrect Alignment");

    Block* Get() noexcept {
        const auto primary = NEWorld::System::Memory::GetBlock();
        return new(primary->Data) Block;
    }

    auto Cast(std::byte* const middle) noexcept { return reinterpret_cast<NEWorld::System::Memory::Block*>(middle); }

    void Return(Block* const blk) noexcept {
        NEWorld::System::Memory::ReturnBlock(Cast(reinterpret_cast<std::byte*>(blk)));
    }

    class Bin final {
    public:
        // Workaround for Clang BUG
        constexpr Bin() noexcept { } // NOLINT

        Bin(Bin&& r) noexcept
                :_Count(r._Count), _First(r._First), _Last(r._Last) { r.Reset(); }

        Bin& operator=(Bin&& r) noexcept {
            if (this!=std::addressof(r)) {
                _Count = r._Count;
                _First = r._First;
                _Last = r._Last;
                r.Reset();
            }
            return *this;
        }

        Bin(const Bin&) = delete;

        Bin& operator=(const Bin&) = delete;

        ~Bin() noexcept = default;

        void Add(Block* const blk) noexcept {
            ++_Count;
            (_First ? _Last->Next : _First) = blk;
            (_Last = blk)->Next = nullptr; // Same as bellow
        }

        void Merge(Bin&& bin) noexcept {
            _Count += bin._Count;
            (_First ? _Last->Next : _First) = bin._First;
            (_Last = bin._Last)->Next = nullptr; // For noting the end of chain
            bin.Reset();
        }

        [[nodiscard]] Bin Tidy()&& noexcept {
            Bin result{};
            for (auto x = _First; x;) {
                const auto next = x->Next;
                if (CouldRecycle(x)) { Return(x); }
                else { result.Add(x); }
                x = next;
            }
            Reset();
            return result;
        }

        [[nodiscard]] uintptr_t Size() const noexcept { return _Count; }

        [[nodiscard]] bool Empty() const noexcept { return !_Count; }
    private:
        uintptr_t _Count{0};
        Block* _First{nullptr}, * _Last{nullptr};

        void Reset() noexcept {
            _Count = 0;
            _First = _Last = nullptr;
        }

        static bool CouldRecycle(const Block* const blk) noexcept { return blk->Alloc==blk->Dealloc.load(); }
    };

    class Central final {
    public:
        ~Central() noexcept { Collect(); }

        void Collect() noexcept {
            CollectG1();
            CollectEden();
        }

        void CollectMerge(Bin&& bin) noexcept { CollectMergeG1(std::move(bin)); }
    private:
        Bin _G1, _Eden;
        SpinLock _G1Lock, _EdenLock;
        uintptr_t _G1VacateThreshold = G1VacateThresholdThread*std::thread::hardware_concurrency();

        Bin VacateCheck() noexcept { return _G1.Size()>=_G1VacateThreshold ? std::move(_G1) : Bin(); }

        void CollectG1() noexcept {
            _G1Lock.Enter();
            auto bin = std::move(_G1);
            _G1Lock.Leave();
            CollectMergeEden(std::move(bin));
        }

        void CollectEden() noexcept {
            _EdenLock.Enter();
            auto bin = std::move(_Eden);
            _EdenLock.Leave();
            CollectMergeEden(std::move(bin));
        }

        void CollectMergeG1(Bin&& bin) noexcept {
            auto tidy = std::move(bin).Tidy();
            if (!tidy.Empty()) {
                _G1Lock.Enter();
                _G1.Merge(std::move(tidy));
                auto vacated = VacateCheck();
                _G1Lock.Leave();
                if (!vacated.Empty()) { CollectMergeEden(std::move(vacated)); }
            }
        }

        void CollectMergeEden(Bin&& bin) noexcept {
            auto tidy = std::move(bin).Tidy();
            if (!tidy.Empty()) {
                _EdenLock.Enter();
                _Eden.Merge(std::move(tidy));
                _EdenLock.Leave();
            }
        }
    };

    NEWorld::ServiceLocation<Central> GCentral;

    class G0 {
    public:
        ~G0() noexcept { Collect(); }

        void Return(const uintptr_t blk) noexcept {
            Push(reinterpret_cast<Block*>(blk));
            CollectIfNeeded();
        }

        void Collect() noexcept { Vacate(); }
    private:
        Bin _Bin;

        void Push(Block* blk) noexcept { _Bin.Add(blk); }

        void CollectIfNeeded() noexcept { if (_Bin.Size()>=G0VacateThreshold) { Vacate(); }}

        void Vacate() noexcept { GCentral.Get().CollectMerge(std::move(_Bin)); }
    };

    class ThreadAllocator final {
    public:
        ThreadAllocator() noexcept { SetupBlock(); }
        ThreadAllocator(ThreadAllocator&&) = delete;
        ThreadAllocator(const ThreadAllocator&) = delete;
        ThreadAllocator& operator=(ThreadAllocator&&) = delete;
        ThreadAllocator& operator=(const ThreadAllocator&) = delete;
        ~ThreadAllocator() noexcept { _G0.Return(_Block); }

        void Collect() noexcept { _G0.Collect(); }

        void* AllocateUnsafe(uintptr_t size) noexcept {
            static constexpr auto AlignMask = alignof(std::max_align_t)-1;
            static constexpr auto AlignRev = ~AlignMask;
            if (size & AlignMask) { size = (size & AlignRev)+alignof(std::max_align_t); }
            for (;;) {
                if (const auto newHead = _Head+size; newHead>=NEWorld::System::Memory::BlockEnd) {
                    SwapBlock();
                }
                else {
                    MarkAlloc(_Block);
                    const auto res = _Block+_Head;
                    _Head = newHead;
                    return reinterpret_cast<void*>(res);
                }
            }
        }

        static ThreadAllocator& Instance() noexcept {
            static thread_local auto trd = std::make_unique<ThreadAllocator>();
            return *trd;
        }
    private:
        uintptr_t _Block{}, _Head{};
        G0 _G0;

        static constexpr uintptr_t AllocStart = NEWorld::System::Memory::Amd64CpuCacheSize*2;

        static uintptr_t Obtain() noexcept { return reinterpret_cast<uintptr_t>(Get()); }

        static void MarkAlloc(const uintptr_t blk) noexcept { ++reinterpret_cast<Block*>(blk)->Alloc; }

        void SetupBlock() noexcept {
            _Block = Obtain();
            _Head = AllocStart;
        }

        void SwapBlock() noexcept {
            _G0.Return(_Block);
            SetupBlock();
        }
    };

    struct ServiceAllocate final : public NEWorld::Object {
        ServiceAllocate() noexcept { new (&GCentral.x) Central; }

        ~ServiceAllocate() noexcept override { GCentral.Get().~Central(); }
    };

    class Sweeper final : public CycleTask {
    public:
        Sweeper() noexcept : CycleTask(std::chrono::seconds(1)) {}

        void OnTimer() noexcept override { GCentral.Get().Collect(); }
    };

    NEWorld::ServiceLocation<Sweeper> STSweeper; // Use after free, but does not matter

    struct ServiceSweep final : public NEWorld::Object {
        ServiceSweep() noexcept {
            new (&STSweeper.x) Sweeper;
            STSweeper.Get().Enable();
        }

        ~ServiceSweep() noexcept override {
            STSweeper.Get().Disable();
            STSweeper.Get().~Sweeper();
            GCentral.Get().Collect();
        }
    private:
        NEWorld::ServiceHandle _Timer { "org.newinfinideas.nrt.cfx.timer" };
        NEWorld::ServiceHandle _Alloc { "org.newinfinideas.nrt.cfx.temp_alloc.a" };
    };

    NW_MAKE_SERVICE(ServiceAllocate, "org.newinfinideas.nrt.cfx.temp_alloc.a", 0.0, _SAlloc)
    NW_MAKE_SERVICE(ServiceSweep, "org.newinfinideas.nrt.cfx.temp_alloc", 0.0, _SSweep)
}

void Temp::Collect() noexcept {
    GCentral.Get().Collect();
    ThreadAllocator::Instance().Collect();
}

void* Temp::AllocateUnsafe(const uintptr_t size) noexcept { return ThreadAllocator::Instance().AllocateUnsafe(size); }
