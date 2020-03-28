#pragma once
#if defined(_WIN32) || defined(__CYGWIN__)
#    ifdef __GNUC__
#        define EXPORT __attribute__ ((dllexport))
#        define IMPORT __attribute__ ((dllimport))
#    else
#        define EXPORT __declspec(dllexport)
#        define IMPORT __declspec(dllimport)
#    endif
#else
#    define EXPORT __attribute__ ((visibility ("default")))
#    define IMPORT
#endif

#if defined(_MSC_VER)
#pragma warning(disable: 4251)
#ifndef _ENABLE_ATOMIC_ALIGNMENT_FIX
#    define _ENABLE_ATOMIC_ALIGNMENT_FIX
#endif
#ifndef _ENABLE_EXTENDED_ALIGNED_STORAGE
#    define _ENABLE_EXTENDED_ALIGNED_STORAGE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#endif
#endif
