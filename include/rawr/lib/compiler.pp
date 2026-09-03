//// rawr/lib/compiler.pp.
// Macro utilities for ergonomic compiler gating.
#pragma once

#include "rawr/lib/detection.pp"
#include "rawr/lib/attributes.pp"
#include "rawr/lib/pp.pp"

#if RAWR_COMPILER_MSVC

    #define RAWR_MSVC(...)                    __VA_ARGS__
    #define RAWR_NOT_MSVC(...)
    #define RAWR_MSVC_OR(MSVC, NotMSVC)       MSVC

    #define RAWR_MSVC_COND(Cond, ...)         RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #define RAWR_MSVC_IF(Cond, True, False)   RAWR_PP_IF(Cond, True, False)
    #define RAWR_MSVC_IF_S(Cond, True, False) RAWR_PP_STRIP(RAWR_PP_IF(Cond, True, False))
    #define RAWR_MSVC_PRAGMA(...)             __pragma(__VA_ARGS__)

    #define RAWR_MSVC_INTRIN(Cond, Name, ...) \
        RAWR_PP_IF(Cond, \
            extern "C" { auto Name __VA_ARGS__; } __pragma(intrinsic(Name)), \
                         auto Name __VA_ARGS__ \
        )
#else
    #define RAWR_MSVC(...)
    #define RAWR_NOT_MSVC(...)                __VA_ARGS__
    #define RAWR_MSVC_OR(MSVC, NotMSVC)       NotMSVC

    #define RAWR_MSVC_COND(Cond, ...)
    #define RAWR_MSVC_IF(Cond, True, False)
    #define RAWR_MSVC_IF_S(Cond, True, False)
    #define RAWR_MSVC_PRAGMA(...)

    #define RAWR_MSVC_INTRIN(Cond, Name, ...) auto Name __VA_ARGS__
#endif

#if RAWR_COMPILER_FAMILY_GNU
    #define RAWR_GNU(...)                    __VA_ARGS__
    #define RAWR_NOT_GNU(...)
    #define RAWR_GNU_OR(GNU, NotGNU)         GNU

    #define RAWR_GNU_COND(Cond, ...)         RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #define RAWR_GNU_IF(Cond, True, False)   RAWR_PP_IF(Cond, True, False)
    #define RAWR_GNU_IF_S(Cond, True, False) RAWR_PP_STRIP(RAWR_PP_IF(Cond, True, False))
    #define RAWR_GNU_PRAGMA(...)             RAWR_RAW_PRAGMA(__VA_ARGS__)
#else
    #define RAWR_GNU(...)
    #define RAWR_NOT_GNU(...)                __VA_ARGS__
    #define RAWR_GNU_OR(GNU, NotGNU)         NotGnu

    #define RAWR_GNU_COND(Cond, ...)
    #define RAWR_GNU_IF(Cond, True, False)
    #define RAWR_GNU_IF_S(Cond, True, False)
    #define RAWR_GNU_PRAGMA(...)
#endif

#if RAWR_COMPILER_CLANG
    #define RAWR_CLANG(...)                    __VA_ARGS__
    #define RAWR_NOT_CLANG(...)
    #define RAWR_CLANG_OR(Clang, NotClang)     Clang

    #define RAWR_CLANG_COND(Cond, ...)         RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #define RAWR_CLANG_IF(Cond, True, False)   RAWR_PP_IF(Cond, True, False)
    #define RAWR_CLANG_IF_S(Cond, True, False) RAWR_PP_STRIP(RAWR_PP_IF(Cond, True, False))
    #define RAWR_CLANG_PRAGMA(...)             RAWR_RAW_PRAGMA(__VA_ARGS__)
#else
    #define RAWR_CLANG(...)
    #define RAWR_NOT_CLANG(...)                __VA_ARGS__
    #define RAWR_CLANG_OR(Clang, NotClang)     NotClang

    #define RAWR_CLANG_COND(Cond, ...)
    #define RAWR_CLANG_IF(Cond, True, False)
    #define RAWR_CLANG_IF_S(Cond, True, False)
    #define RAWR_CLANG_PRAGMA(...)
#endif

#if RAWR_COMPILER_GCC
    #define RAWR_GCC(...)                    __VA_ARGS__
    #define RAWR_NOT_GCC(...)
    #define RAWR_GCC_OR(GCC, NotGCC)         GCC

    #define RAWR_GCC_COND(Cond, ...)         RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #define RAWR_GCC_IF(Cond, True, False)   RAWR_PP_IF(Cond, True, False)
    #define RAWR_GCC_IF_S(Cond, True, False) RAWR_PP_STRIP(RAWR_PP_IF(Cond, True, False))
    #define RAWR_GCC_PRAGMA(...)             RAWR_RAW_PRAGMA(__VA_ARGS__)
#else
    #define RAWR_GCC(...)
    #define RAWR_NOT_GCC(...)                __VA_ARGS__
    #define RAWR_GCC_OR(GCC, NotGCC)         NotGCC

    #define RAWR_GCC_COND(Cond, ...)
    #define RAWR_GCC_IF(Cond, True, False)
    #define RAWR_GCC_IF_S(Cond, True, False)
    #define RAWR_GCC_PRAGMA(...)
#endif
