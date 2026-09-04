#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/attributes.pp"
#endif
// The macros in this file are defined as the lower level constructs
// directly instead of defining, say, RAWR_FLATTEN as RAWR_ATTIBUTE(flatten),
// so that theres less expansions and more consisten and readable errors.
// No one likes macro expansion puke.
#pragma once

#include "rawr/lib/detection.pp"

#define RAWR_RAW_PRAGMA(x) _Pragma(#x)
// Clang-cl and mingw support __declspec, if you want to use
// it here it is. These are the escape hatches for special cases.
// Note that:
//     RAWR_DECLSPEC  = __declspec    -> Only defined on MSVC
//     RAWR_ATTRIBUTE = __attribute__ -> Only defined outside of MSVC.
// While these are always defined any may expand into invalid things
// if you don't know what you're doing.
#define RAWR_RAW_DECLSPEC(x)  __declspec(x)
#define RAWR_RAW_ATTRIBUTE(x) __attribute__((X))

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

    // /alternatename is the MSVC linker-level symbol alias mechanism.
    // Usage: RAWR_SYMBOL_ALIAS("target") on the declaration,
    //        then RAWR_SYMBOL_ALIAS_PRAGMA("cname", "target") at namespace scope.
    #define RAWR_ASM_ALIAS(sym)
    #define RAWR_SYMBOL_ALIAS_PRAGMA(from, to) __pragma(comment(linker, "/alternatename:" from "=" to))
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

    #define RAWR_ASM_ALIAS(sym) asm(sym)
    #define RAWR_SYMBOL_ALIAS_PRAGMA(from, to)
#endif

#if RAWR_COMPILER_CLANG
    #define RAWR_ASSUME(cond) __builtin_assume(cond)
#elif RAWR_COMPILER_GCC
    #define RAWR_ASSUME(cond) do { if (!(cond)) __builtin_unreachable(); } while(0)
#elif RAWR_COMPILER_MSVC
    #define RAWR_ASSUME(cond) __assume(cond)
#endif
