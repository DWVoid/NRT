#pragma once

#include "Core/Slangs.h"

#ifdef NRTOGL_BUILD
#define NRTOGL_API EXPORT
#else
#define NRTOGL_API IMPORT
#endif
