// Macro utilities for ergonomic compiler gating.
#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.lib.compiler.pp
#endif

#include "rawr/detection/compiler.pp"
#include "rawr/lib/pp.pp"

// The macros in this file are defined as the lower level constructs
// directly instead of defining, say, RAWR_FLATTEN as RAWR_ATTIBUTE(flatten),
// so that theres less expansions and more consistent and readable errors.
// No one likes macro expansion puke.


// --- Attribute stuff ---


#define RAWR_RAW_PRAGMA(x) _Pragma(#x)
// Clang-cl and mingw support __declspec, if you want to use
// it, here it is. These are the escape hatches for special cases.
// Note that:
//     RAWR_DECLSPEC  = __declspec    -> Only defined on MSVC
//     RAWR_ATTRIBUTE = __attribute__ -> Only defined outside of MSVC.
// While these (RAWR_RAW_) are always defined any may expand into invalid things
// if you don't know what you're doing.
#define RAWR_RAW_DECLSPEC(x)  __declspec(x)
#define RAWR_RAW_ATTRIBUTE(x) __attribute__((x))

#if RAWR_COMPILER_MSVC
    #define RAWR_DECLSPEC(x)   __declspec(x)
    #define RAWR_ATTRIBUTE(x)
    #define RAWR_PRAGMA(x)     __pragma(x) // Can use __pragma directly without stringification.

    #define RAWR_UNREACHABLE   __assume(false)
    #define RAWR_NORETURN      __declspec(noreturn)
    #define RAWR_HIDDEN
    #define RAWR_ALWAYS_INLINE __forceinline
    #define RAWR_FLATTEN       // no MSVC equivalent — accept the cost
    #define RAWR_NAKED         // not supported on x64 MSVC at all
    #define RAWR_WEAK

    #define RAWR_ASM(...)
    #define RAWR_ASMV(...)
    // /alternatename is the MSVC linker-level symbol alias mechanism.
    // Usage: RAWR_ASM("target") on the declaration,
    //        then RAWR_ALTERNATENAME("cname", "target") at namespace scope.
    #define RAWR_ALTERNATENAME(from, to) __pragma(comment(linker, "/alternatename:" from "=" to))
#else
    #define RAWR_DECLSPEC(x)
    #define RAWR_ATTRIBUTE(x)  __attribute__((x))
    #define RAWR_PRAGMA(x)     RAWR_RAW_PRAGMA(x) // Needs deffered resolution.

    #define RAWR_UNREACHABLE   __builtin_unreachable()
    #define RAWR_NORETURN      __attribute__((noreturn))
    #define RAWR_HIDDEN        __attribute__((visibility("hidden")))
    #define RAWR_ALWAYS_INLINE __attribute__((always_inline)) inline
    #define RAWR_FLATTEN       __attribute__((flatten))
    #define RAWR_NAKED         __attribute__((naked))
    #define RAWR_WEAK          __attribute__((weak))

    #define RAWR_ASM(...)  __asm__(__VA_ARGS__)
    #define RAWR_ASMV(...) __asm__ __volatile__(__VA_ARGS__)
    #define RAWR_ALTERNATENAME(from, to)
#endif

#if RAWR_COMPILER_CLANG
    #define RAWR_ASSUME(cond) __builtin_assume(cond)
#elif RAWR_COMPILER_GCC
    #define RAWR_ASSUME(cond) do { if (!(cond)) __builtin_unreachable(); } while(0)
#elif RAWR_COMPILER_MSVC
    #define RAWR_ASSUME(cond) __assume(cond)
#endif

#if RAWR_COMPILER_GCC || (RAWR_COMPILER_CLANG && RAWR_COMPILER_VERSION_MAJOR >= 14)
    #define RAWR_ATTR_ERROR(Err) [[gnu::error(Err)]]
#else
    #define RAWR_ATTR_ERROR(Err)
#endif


// --- General utilities ---


#if RAWR_COMPILER_MSVC
    #define RAWR_MSVC(...)                    __VA_ARGS__
    #define RAWR_NOT_MSVC(...)
    #define RAWR_MSVC_ELSE(MSVC, NotMSVC)     MSVC
    #define RAWR_MSVC_AND(Cond, ...)          RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #define RAWR_MSVC_PRAGMA(...)             __pragma(__VA_ARGS__)

    #define RAWR_MSVC_INTRIN(Cond, Name, ...) \
        RAWR_PP_IF(Cond, \
            extern "C" { auto Name __VA_ARGS__; } __pragma(intrinsic(Name)), \
                         auto Name __VA_ARGS__ \
        )
#else
    #define RAWR_MSVC(...)
    #define RAWR_NOT_MSVC(...)                __VA_ARGS__
    #define RAWR_MSVC_ELSE(MSVC, NotMSVC)     NotMSVC
    #define RAWR_MSVC_AND(Cond, ...)
    #define RAWR_MSVC_PRAGMA(...)

    #define RAWR_MSVC_INTRIN(Cond, Name, ...) auto Name __VA_ARGS__
#endif

#if RAWR_COMPILER_FAMILY_GNU
    #define RAWR_GNU(...)              __VA_ARGS__
    #define RAWR_NOT_GNU(...)
    #define RAWR_GNU_ELSE(GNU, NotGNU) GNU
    #define RAWR_GNU_AND(Cond, ...)    RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #define RAWR_GNU_PRAGMA(...)       RAWR_RAW_PRAGMA(__VA_ARGS__)
#else
    #define RAWR_GNU(...)
    #define RAWR_NOT_GNU(...)          __VA_ARGS__
    #define RAWR_GNU_ELSE(GNU, NotGNU) NotGnu
    #define RAWR_GNU_AND(Cond, ...)
    #define RAWR_GNU_PRAGMA(...)
#endif

#if RAWR_COMPILER_CLANG
    #define RAWR_CLANG(...)                  __VA_ARGS__
    #define RAWR_NOT_CLANG(...)
    #define RAWR_CLANG_ELSE(Clang, NotClang) Clang
    #define RAWR_CLANG_AND(Cond, ...)        RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #define RAWR_CLANG_PRAGMA(...)           RAWR_RAW_PRAGMA(__VA_ARGS__)
#else
    #define RAWR_CLANG(...)
    #define RAWR_NOT_CLANG(...)              __VA_ARGS__
    #define RAWR_CLANG_ELSE(Clang, NotClang) NotClang
    #define RAWR_CLANG_AND(Cond, ...)
    #define RAWR_CLANG_PRAGMA(...)
#endif

#if RAWR_COMPILER_GCC
    #define RAWR_GCC(...)              __VA_ARGS__
    #define RAWR_NOT_GCC(...)
    #define RAWR_GCC_ELSE(GCC, NotGCC) GCC
    #define RAWR_GCC_AND(Cond, ...)    RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #define RAWR_GCC_PRAGMA(...)       RAWR_RAW_PRAGMA(__VA_ARGS__)
#else
    #define RAWR_GCC(...)
    #define RAWR_NOT_GCC(...)          __VA_ARGS__
    #define RAWR_GCC_ELSE(GCC, NotGCC) NotGCC
    #define RAWR_GCC_AND(Cond, ...)
    #define RAWR_GCC_PRAGMA(...)
#endif
