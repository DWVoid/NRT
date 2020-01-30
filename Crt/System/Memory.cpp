#include "System/Memory.h"
#include <new>
#include <cstdlib>

#if __has_include(<Windows.h>)
#include "System/Local/WindowsLess.h"
#define SYSTEM_WINDOWS
namespace {
    struct HouseKeep {
        uintptr_t ReleaseBase = 0;
    };
}
#else
#define SYSTEM_POSIX
#endif

namespace NEWorld::System::Memory {
    Block* GetBlock() noexcept {
        #ifdef SYSTEM_POSIX
        void* mem;
        const int ret = posix_memalign(&mem, BlockSize, BlockSize);
        return new(mem) Block;
        #elif defined(SYSTEM_WINDOWS)
        static constexpr auto AlignMask = BlockSize - 1;
        static constexpr auto AlignRev = ~AlignMask;
        const auto base = reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr, BlockSize << 1, MEM_RESERVE,
                PAGE_READWRITE));
        const auto loc = base & AlignMask ? (base & AlignRev) + BlockSize : base;
        const auto blk = new(VirtualAlloc(reinterpret_cast<LPVOID>(loc), BlockSize, MEM_COMMIT, PAGE_READWRITE)) Block;
        const auto kp = reinterpret_cast<HouseKeep*>(blk->Reserved);
        kp->ReleaseBase = base;
        return blk;
        #endif
    }

    void ReturnBlock(Block* blk) noexcept {
#ifdef SYSTEM_POSIX
        free(blk);
#elif defined(SYSTEM_WINDOWS)
        VirtualFree(reinterpret_cast<LPVOID>(reinterpret_cast<HouseKeep*>(blk.Reserved)->ReleaseBase), 0, MEM_RELEASE);
#endif
    }

    void SetPooling(bool enable) noexcept {}
}
