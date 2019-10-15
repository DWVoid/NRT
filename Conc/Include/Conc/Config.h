#pragma once

#include "Core/Slangs.h"

#ifdef NRTCONC_BUILD
#define NRTCONC_API EXPORT
#else
#define NRTCONC_API IMPORT
#endif
