#include "Cfx/Utilities/Timings.h"

#if __has_include(<Windows.h>)
#include "Cfx/Utilities/System/WindowsLess.h"
#define HAS_WIN32_CLOCK 1
#elif __has_include(<mach/mach_time.h>)
#define HAVE_MACH_ABSOLUTE_TIME 1
#include <mach/mach_time.h>
#include <Compile/DyldExec.h>
#elif __has_include(<time.h>)
#include <ctime>
#if defined(CLOCK_MONOTONIC)
#define HAVE_CLOCK_MONOTONIC 1
#endif
#endif


namespace {
#ifndef HAS_WIN32_CLOCK
    constexpr int tccSecondsToNanoSeconds = 1000000000;
#endif

#if HAVE_MACH_ABSOLUTE_TIME
    mach_timebase_info_data_t _Base;

    NW_DY_LD_EXEC([]() noexcept { mach_timebase_info(&_Base); })
#endif
}

bool QueryPerformanceCounter(int64_t* lpPerformanceCount) noexcept {
#if HAVE_MACH_ABSOLUTE_TIME
    *lpPerformanceCount = mach_absolute_time();
	return true;
#elif HAVE_CLOCK_MONOTONIC
    struct timespec ts{};
    int result = clock_gettime(CLOCK_MONOTONIC, &ts);
    if (result!=0) {
        return false;
    }
    else {
        *lpPerformanceCount=((int64_t)(ts.tv_sec)*(int64_t)(tccSecondsToNanoSeconds))+(int64_t)(ts.tv_nsec);
    }
	return true;
#elif HAS_WIN32_CLOCK
    return static_cast<bool>(QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(lpPerformanceCount)));
#else
#error "NO PROPPER CLOCK SUPPORT"
#endif
}

bool QueryPerformanceFrequency(int64_t* lpFrequency) noexcept {
#if HAVE_MACH_ABSOLUTE_TIME
    if (_Base.denom==0) {
        return false;
    }
    else {
        *lpFrequency = ((int64_t) (tccSecondsToNanoSeconds)*(int64_t) (_Base.denom))/(int64_t) (_Base.numer);
    }
	return true;
#elif HAVE_CLOCK_MONOTONIC
    *lpFrequency = tccSecondsToNanoSeconds;
	return true;
#elif HAS_WIN32_CLOCK
    return static_cast<bool>(QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(lpFrequency)));
#else
#error "NO PROPPER CLOCK SUPPORT"
#endif
}
