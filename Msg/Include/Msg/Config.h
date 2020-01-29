#pragma once

#include "Compile/Slangs.h"

#ifdef NRTMSG_BUILD
#define NRTMSG_API EXPORT
#else
#define NRTMSG_API IMPORT
#endif
