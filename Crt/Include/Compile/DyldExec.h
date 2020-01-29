#pragma once

namespace NEWorld {
    using DyldExec = void(*)() noexcept;
}

#define NW_CONCAT_2I(a, b) a ## b
#define NW_CONCAT_2(a, b) NW_CONCAT_2I(a, b)
#define NW_UNIQUE_NAME NW_CONCAT_2(NEWorldName, __LINE__)

#if defined(__GNUC__) || defined(__clang__)
namespace NEWorld::InterOp { inline void DyldInvoke(const DyldExec fn) noexcept { fn(); } }
#define NW_DY_LD_EXEC(x)\
    namespace { [[gnu::constructor]] void NW_CONCAT_2(NW_UNIQUE_NAME, C)() noexcept { ::NEWorld::InterOp::DyldInvoke((x)); } }
#define NW_DY_UD_EXEC(x)\
    namespace { [[gnu::destructor]] void NW_CONCAT_2(NW_UNIQUE_NAME, D)() noexcept { ::NEWorld::InterOp::DyldInvoke((x)); } }
#else
#ifndef _MSC_VER
#warning "unsupported compiler, execution result is not garenteed"
#endif
namespace NEWorld::InterOp {
    struct DyldExecutor { explicit DyldExecutor(const DyldExec fn) noexcept { fn(); }}; // NOLINT

    struct DyUdExecutor {
        explicit DyUdExecutor(const DyldExec fn) noexcept: mExec(fn) {}
        ~DyUdExecutor() noexcept { mExec(); }
    private:
        DyldExec mExec;
    };
}
#define NW_DY_LD_EXEC(x) namespace { ::NEWorld::InterOp::DyldExecutor NW_UNIQUE_NAME##C { (x) }; }
#define NW_DY_UD_EXEC(x) namespace { ::NEWorld::InterOp::DyUdExecutor NW_UNIQUE_NAME##D { (x) }; }
#endif
