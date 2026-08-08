//// rawr/cxx/intrin/macros.hpp
#pragma region rawr/cxx/intrin/macros.hpp

#ifndef RAWR_UNITY
    #pragma once
    #include "rawr/lib/detection_macros.hpp"
#endif

#if RAWR_COMPILER_MSVC
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

#pragma endregion rawr/cxx/intrin/macros.hpp
