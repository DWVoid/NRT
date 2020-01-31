#pragma once

#include "Compile/Config.h"
#include <cstdint>

namespace NEWorld::System::Memory {
    static constexpr uintptr_t Amd64CpuCacheSize = 64;
    static constexpr uintptr_t BlockSize = 4u << 20u; // 4MiB
    static constexpr uintptr_t BlockEnd = BlockSize - Amd64CpuCacheSize; // Reverse the last cache line for housekeeping

    struct Block {
        char Data[BlockEnd];
        char Reserved[Amd64CpuCacheSize];
    };

    NRTCRT_API Block* GetBlock() noexcept;
    NRTCRT_API void ReturnBlock(Block* blk) noexcept;
    NRTCRT_API void SetPooling(bool enable) noexcept;
}
