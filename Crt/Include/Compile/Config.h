#pragma once

#include "Compile/Slangs.h"

#ifdef NRTCRT_BUILD
#define NRTCRT_API EXPORT
#else
#define NRTCRT_API IMPORT
#endif
