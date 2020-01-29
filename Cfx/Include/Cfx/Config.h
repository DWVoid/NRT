#pragma once

#include "Compile/Slangs.h"

#ifdef NRTCORE_BUILD
#define NRTCORE_API EXPORT
#else
#define NRTCORE_API IMPORT
#endif
