#include <cstddef>
#include <cstdlib>
#include <cassert>
#include "Cfx/Utilities/TempAlloc.h"
#include "Cfx/Threading/SpinLock.h"
#include "Cfx/Threading/Micro/Timer.h"
#include "Cfx/Threading/Micro/ThreadPool.h"

#if __has_include(<Windows.h>)
#include "Cfx/Utilities/System/WindowsLess.h"
#define SYSTEM_WINDOWS
#else
#define SYSTEM_POSIX
#endif

namespace {
    class BlockManager : public Temp {
    public:
        void Return(const uintptr_t blk) noexcept { _G0.Push(reinterpret_cast<MemBlock*>(blk)); }

        void CheckTidyLocal() noexcept { _G0.CollectIfNeeded(); }

        void CollectLocal() noexcept { _G0.Collect(); }

        static uintptr_t Obtain() noexcept { return reinterpret_cast<uintptr_t>(AllocateBlock()); }

        static void MarkAlloc(const uintptr_t blk) noexcept { ++reinterpret_cast<MemBlock*>(blk)->s.Alloc; }

        static void Collect() { Central::Instance().Collect(); }
    protected:
        static constexpr uintptr_t AllocStart = Amd64CpuCacheSize * 2;
        static constexpr uintptr_t BlockSize = 4u << 20u;
    private:
        static constexpr uintptr_t G0VacateThreshold = 5;
        static constexpr uintptr_t G1VacateThresholdThread = 2;

        union MemBlock {
            struct S {
                uint32_t Alloc = 0;
                MemBlock* Next = nullptr;
                uintptr_t ReleaseBase = 0;
                alignas(Amd64CpuCacheSize) std::atomic_uint32_t Dealloc{0};
            } s;

			explicit constexpr MemBlock(const uintptr_t rel = 0) noexcept: s{}{}

            char __UU_MEM__[BlockSize];
        };

        static_assert(offsetof(MemBlock, s.Dealloc) == Amd64CpuCacheSize, "Incorrect Alignment");

        static MemBlock* AllocateBlock() noexcept {
#ifdef SYSTEM_POSIX
            void* mem;
            int ret = posix_memalign(&mem, BlockSize, BlockSize);
            assert(ret == 0);
            return new(mem) MemBlock;
#elif defined(SYSTEM_WINDOWS)
            static constexpr auto AlignMask = BlockSize - 1;
            static constexpr auto AlignRev = ~AlignMask;
            const auto base = reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr, BlockSize << 1, MEM_RESERVE,
                                                                       PAGE_READWRITE));
            const auto loc = base & AlignMask ? (base & AlignRev) + BlockSize : base;
            return new(VirtualAlloc(reinterpret_cast<LPVOID>(loc), BlockSize, MEM_COMMIT, PAGE_READWRITE)) MemBlock
                (base);
#endif
        }

        static void FreeBlock(MemBlock* blk) noexcept {
#ifdef SYSTEM_POSIX
            free(blk);
#elif defined(SYSTEM_WINDOWS)
            VirtualFree(reinterpret_cast<LPVOID>(blk->s.ReleaseBase), 0, MEM_RELEASE);
#endif
        }

        class Bin {
        public:
            // Workaround for Clang BUG
            constexpr Bin() noexcept {}; // NOLINT

            Bin(Bin&& r) noexcept : _Count(r._Count), _First(r._First), _Last(r._Last) { r.Reset(); }

            Bin& operator =(Bin&& r) noexcept {
                if (this != std::addressof(r)) {
                    _Count = r._Count;
                    _First = r._First;
                    _Last = r._Last;
                    r.Reset();
                }
                return *this;
            }

            Bin(const Bin&) = delete;

            Bin& operator =(const Bin&) = delete;

            ~Bin() noexcept = default;

            void Add(MemBlock* blk) noexcept {
                ++_Count;
                (_First ? _Last->s.Next : _First) = blk;
                (_Last = blk)->s.Next = nullptr; // Same as bellow
            }

            void Merge(Bin&& bin) noexcept {
                _Count += bin._Count;
                (_First ? _Last->s.Next : _First) = bin._First;
                (_Last = bin._Last)->s.Next = nullptr; // For noting the end of chain
                bin.Reset();
            }

            [[nodiscard]] Bin Tidy() && noexcept {
                Bin result{};
                for (auto x = _First; x;) {
                    const auto next = x->s.Next;
                    if (CouldRecycle(x)) { FreeBlock(x); }
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
            MemBlock *_First{nullptr}, *_Last{nullptr};

            void Reset() noexcept {
                _Count = 0;
                _First = _Last = nullptr;
            }

            static bool CouldRecycle(MemBlock* blk) noexcept { return blk->s.Alloc == blk->s.Dealloc.load(); }
        };

        class Central final : CycleTask {
        public:
            Central() noexcept: CycleTask(std::chrono::seconds(1)) { Enable(); }

            Central(Central&&) = delete;

            Central(const Central&) = delete;

            Central& operator=(Central&&) = delete;

            Central& operator=(const Central&) = delete;

            ~Central() noexcept override { Collect(); }

            void Collect() noexcept {
                CollectG1();
                CollectEden();x
            }

            void CollectMerge(Bin&& bin) noexcept { CollectMergeG1(std::move(bin)); }

            void StopEdenTimedTidying() noexcept { Disable(); }

            static Central& Instance() noexcept {
                static Central instance;
                return instance;
            }

        private:
            Bin _G1, _Eden;
            SpinLock _G1Lock, _EdenLock;
            uintptr_t _G1VacateThreshold = G1VacateThresholdThread * std::thread::hardware_concurrency();

            void OnTimer() noexcept override { Collect(); }

            Bin VacateCheck() noexcept { return _G1.Size() >= _G1VacateThreshold ? std::move(_G1) : Bin(); }

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

        class G0 final {
        public:
            // Workaround for Clang BUG
            G0() noexcept {} // NOLINT
            G0(G0&&) = delete;
            G0(const G0&) = delete;
            G0& operator=(G0&&) = delete;
            G0& operator=(const G0&) = delete;
            ~G0() noexcept { _Central.CollectMerge(std::move(_Bin)); }

            void Push(MemBlock* blk) noexcept { _Bin.Add(blk); }

            void CollectIfNeeded() noexcept { if (_Bin.Size() >= G0VacateThreshold) { Vacate(); } }

            void Collect() noexcept { Vacate(); }
        private:
            Bin _Bin;
            Central& _Central = Central::Instance();
            void Vacate() noexcept { _Central.CollectMerge(std::move(_Bin)); }
        } _G0;
    };

    class Implementation final : public BlockManager {
    public:
        static void* AllocateUnsafe(const uintptr_t size) noexcept { return Get()->AllocateUnsafe(size); }

        static void Collect() noexcept {
            Get()->Collect();
            BlockManager::Collect();
        }

        static void DisableSweep() noexcept { Get()->DisableSweeping(); }

        static void EnableSweep() noexcept { Get()->EnableSweeping(); }

        static void Nothing() noexcept { Get(); }
    private:
        class Local final {
        public:
            Local() noexcept { SetupBlock(); }
            Local(Local&&) = delete;
            Local(const Local&) = delete;
            Local& operator=(Local&&) = delete;
            Local& operator=(const Local&) = delete;
            ~Local() noexcept { _Mgr.Return(_Block); }

            void SetupBlock() noexcept {
                _Block = Obtain();
                _Head = AllocStart;
            }

            void SwapBlock() noexcept {
                _Mgr.Return(_Block);
                SetupBlock();
            }

            void Collect() noexcept { _Mgr.CollectLocal(); }

            void* AllocateUnsafe(uintptr_t size) noexcept {
                static constexpr auto AlignMask = alignof(std::max_align_t) - 1;
                static constexpr auto AlignRev = ~AlignMask;
                if (size & AlignMask) { size = (size & AlignRev) + alignof(std::max_align_t); }
                for (;;) {
                    const auto newHead = _Head + size;
                    if (newHead >= BlockSize) {
                        SwapBlock();
                        if (OnDemandTidy) { _Mgr.CheckTidyLocal(); }
                    }
                    else {
                        MarkAlloc(_Block);
                        const auto res = _Block + _Head;
                        _Head = newHead;
                        return reinterpret_cast<void*>(res);
                    }
                }
            }

            void DisableSweeping() noexcept { OnDemandTidy = false; }

            void EnableSweeping() noexcept {
                OnDemandTidy = true;
                _Mgr.CheckTidyLocal();
            }

        private:
            uintptr_t _Block{}, _Head{};
            bool OnDemandTidy{true};
            BlockManager _Mgr;
        };

        static Local* Get() noexcept {
            struct UniqueLocal final {
                UniqueLocal() noexcept
                    : Mem(new Local()) { }

                UniqueLocal(UniqueLocal&&) = delete;
                UniqueLocal(const UniqueLocal&) = delete;
                UniqueLocal& operator=(UniqueLocal&&) = delete;
                UniqueLocal& operator=(const UniqueLocal&) = delete;
                ~UniqueLocal() { delete Mem; }
                [[nodiscard]] Local* Get() const noexcept { return Mem; }
                Local* Mem;
            };
            static thread_local UniqueLocal trd{};
            return trd.Get();
        }
    };
}

namespace Utilities::InterOp {
    void StopOnEdenCollectorStop() noexcept { Implementation::StopEdenTimedTidying(); }

    void NoAsyncG0Tidy() noexcept { Implementation::DisableSweep(); }

    void AsyncTidyLocalG0() noexcept { Implementation::EnableSweep(); }

    void PreHeatTemp() noexcept { Implementation::Nothing(); }
}

void Temp::Collect() noexcept { Implementation::Collect(); }

void* Temp::AllocateUnsafe(const uintptr_t size) noexcept { return Implementation::AllocateUnsafe(size); }
