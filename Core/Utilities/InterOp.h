#pragma once

namespace Utilities::InterOp {
    void PreHeatTemp() noexcept;
    void StopOnEdenCollectorStop() noexcept;
    void StopOnTimerStop() noexcept;
    void StopOnThreadPoolStop() noexcept;
    void NoAsyncG0Tidy() noexcept;
    void AsyncTidyLocalG0() noexcept;
}
