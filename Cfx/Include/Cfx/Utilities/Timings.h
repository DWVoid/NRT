#pragma once

#include <cstdint>
#include "Cfx/Config.h"

NRTCORE_API bool QueryPerformanceCounter(int64_t* lpPerformanceCount) noexcept;
NRTCORE_API bool QueryPerformanceFrequency(int64_t* lpFrequency) noexcept;
