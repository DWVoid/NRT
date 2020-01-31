#pragma once

#include "Compile/Slangs.h"

#ifdef _MSC_VER
#define _ENABLE_EXTENDED_ALIGNED_STORAGE 
#endif
#ifdef NRTCRT_BUILD
#define NRTCRT_API EXPORT
#else
#define NRTCRT_API IMPORT
#endif
