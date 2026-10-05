// GENERATED — do not edit
// Macro utilities for ergonomic compiler gating.

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.lib.compiler.pp
#endif

#include "rawr/detection/compiler.local.pp"
#include "rawr/lib/pp.local.pp"

// The macros in this file are defined as the lower level constructs
// directly instead of defining, say, RAWR_FLATTEN as RAWR_ATTIBUTE(flatten),
// so that theres less expansions and more consistent and readable errors.
// No one likes macro expansion puke.


// --- Attribute stuff ---


#undef RAWR_RAW_PRAGMA
#define RAWR_RAW_PRAGMA(x) _Pragma(#x)
// Clang-cl and mingw support __declspec, if you want to use
// it, here it is. These are the escape hatches for special cases.
// Note that:
//     RAWR_DECLSPEC  = __declspec    -> Only defined on MSVC
//     RAWR_ATTRIBUTE = __attribute__ -> Only defined outside of MSVC.
// While these (RAWR_RAW_) are always defined any may expand into invalid things
// if you don't know what you're doing.
#undef RAWR_RAW_DECLSPEC
#define RAWR_RAW_DECLSPEC(x)  __declspec(x)
#undef RAWR_RAW_ATTRIBUTE
#define RAWR_RAW_ATTRIBUTE(x) __attribute__((x))

#if RAWR_COMPILER_MSVC
    #undef RAWR_DECLSPEC
    #define RAWR_DECLSPEC(x)   __declspec(x)
    #undef RAWR_ATTRIBUTE
    #define RAWR_ATTRIBUTE(x)
    #undef RAWR_PRAGMA
    #define RAWR_PRAGMA(x)     __pragma(x) // Can use __pragma directly without stringification.

    #undef RAWR_UNREACHABLE
    #define RAWR_UNREACHABLE   __assume(false)
    #undef RAWR_NORETURN
    #define RAWR_NORETURN      __declspec(noreturn)
    #undef RAWR_HIDDEN
    #define RAWR_HIDDEN
    #undef RAWR_ALWAYS_INLINE
    #define RAWR_ALWAYS_INLINE __forceinline
    #undef RAWR_FLATTEN
    #define RAWR_FLATTEN       // no MSVC equivalent — accept the cost
    #undef RAWR_NAKED
    #define RAWR_NAKED         // not supported on x64 MSVC at all
    #undef RAWR_WEAK
    #define RAWR_WEAK

    #undef RAWR_ASM
    #define RAWR_ASM(...)
    #undef RAWR_ASMV
    #define RAWR_ASMV(...)
    // /alternatename is the MSVC linker-level symbol alias mechanism.
    // Usage: RAWR_ASM("target") on the declaration,
    //        then RAWR_ALTERNATENAME("cname", "target") at namespace scope.
    #undef RAWR_ALTERNATENAME
    #define RAWR_ALTERNATENAME(from, to) __pragma(comment(linker, "/alternatename:" from "=" to))
#else
    #undef RAWR_DECLSPEC
    #define RAWR_DECLSPEC(x)
    #undef RAWR_ATTRIBUTE
    #define RAWR_ATTRIBUTE(x)  __attribute__((x))
    #undef RAWR_PRAGMA
    #define RAWR_PRAGMA(x)     RAWR_RAW_PRAGMA(x) // Needs deffered resolution.

    #undef RAWR_UNREACHABLE
    #define RAWR_UNREACHABLE   __builtin_unreachable()
    #undef RAWR_NORETURN
    #define RAWR_NORETURN      __attribute__((noreturn))
    #undef RAWR_HIDDEN
    #define RAWR_HIDDEN        __attribute__((visibility("hidden")))
    #undef RAWR_ALWAYS_INLINE
    #define RAWR_ALWAYS_INLINE __attribute__((always_inline)) inline
    #undef RAWR_FLATTEN
    #define RAWR_FLATTEN       __attribute__((flatten))
    #undef RAWR_NAKED
    #define RAWR_NAKED         __attribute__((naked))
    #undef RAWR_WEAK
    #define RAWR_WEAK          __attribute__((weak))

    #undef RAWR_ASM
    #define RAWR_ASM(...)  __asm__(__VA_ARGS__)
    #undef RAWR_ASMV
    #define RAWR_ASMV(...) __asm__ __volatile__(__VA_ARGS__)
    #undef RAWR_ALTERNATENAME
    #define RAWR_ALTERNATENAME(from, to)
#endif

#if RAWR_COMPILER_CLANG
    #undef RAWR_ASSUME
    #define RAWR_ASSUME(cond) __builtin_assume(cond)
#elif RAWR_COMPILER_GCC
    #undef RAWR_ASSUME
    #define RAWR_ASSUME(cond) do { if (!(cond)) __builtin_unreachable(); } while(0)
#elif RAWR_COMPILER_MSVC
    #undef RAWR_ASSUME
    #define RAWR_ASSUME(cond) __assume(cond)
#endif

#if RAWR_COMPILER_GCC || (RAWR_COMPILER_CLANG && RAWR_COMPILER_VERSION_MAJOR >= 14)
    #undef RAWR_ATTR_ERROR
    #define RAWR_ATTR_ERROR(Err) [[gnu::error(Err)]]
#else
    #undef RAWR_ATTR_ERROR
    #define RAWR_ATTR_ERROR(Err)
#endif


// --- General utilities ---


#if RAWR_COMPILER_MSVC
    #undef RAWR_MSVC
    #define RAWR_MSVC(...)                    __VA_ARGS__
    #undef RAWR_NOT_MSVC
    #define RAWR_NOT_MSVC(...)
    #undef RAWR_MSVC_ELSE
    #define RAWR_MSVC_ELSE(MSVC, NotMSVC)     MSVC
    #undef RAWR_MSVC_AND
    #define RAWR_MSVC_AND(Cond, ...)          RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #undef RAWR_MSVC_PRAGMA
    #define RAWR_MSVC_PRAGMA(...)             __pragma(__VA_ARGS__)

    #undef RAWR_MSVC_INTRIN
    #define RAWR_MSVC_INTRIN(Cond, Name, ...) \
        RAWR_PP_IF(Cond, \
            extern "C" { auto Name __VA_ARGS__; } __pragma(intrinsic(Name)), \
                         auto Name __VA_ARGS__ \
        )
#else
    #undef RAWR_MSVC
    #define RAWR_MSVC(...)
    #undef RAWR_NOT_MSVC
    #define RAWR_NOT_MSVC(...)                __VA_ARGS__
    #undef RAWR_MSVC_ELSE
    #define RAWR_MSVC_ELSE(MSVC, NotMSVC)     NotMSVC
    #undef RAWR_MSVC_AND
    #define RAWR_MSVC_AND(Cond, ...)
    #undef RAWR_MSVC_PRAGMA
    #define RAWR_MSVC_PRAGMA(...)

    #undef RAWR_MSVC_INTRIN
    #define RAWR_MSVC_INTRIN(Cond, Name, ...) auto Name __VA_ARGS__
#endif

#if RAWR_COMPILER_FAMILY_GNU
    #undef RAWR_GNU
    #define RAWR_GNU(...)              __VA_ARGS__
    #undef RAWR_NOT_GNU
    #define RAWR_NOT_GNU(...)
    #undef RAWR_GNU_ELSE
    #define RAWR_GNU_ELSE(GNU, NotGNU) GNU
    #undef RAWR_GNU_AND
    #define RAWR_GNU_AND(Cond, ...)    RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #undef RAWR_GNU_PRAGMA
    #define RAWR_GNU_PRAGMA(...)       RAWR_RAW_PRAGMA(__VA_ARGS__)
#else
    #undef RAWR_GNU
    #define RAWR_GNU(...)
    #undef RAWR_NOT_GNU
    #define RAWR_NOT_GNU(...)          __VA_ARGS__
    #undef RAWR_GNU_ELSE
    #define RAWR_GNU_ELSE(GNU, NotGNU) NotGnu
    #undef RAWR_GNU_AND
    #define RAWR_GNU_AND(Cond, ...)
    #undef RAWR_GNU_PRAGMA
    #define RAWR_GNU_PRAGMA(...)
#endif

#if RAWR_COMPILER_CLANG
    #undef RAWR_CLANG
    #define RAWR_CLANG(...)                  __VA_ARGS__
    #undef RAWR_NOT_CLANG
    #define RAWR_NOT_CLANG(...)
    #undef RAWR_CLANG_ELSE
    #define RAWR_CLANG_ELSE(Clang, NotClang) Clang
    #undef RAWR_CLANG_AND
    #define RAWR_CLANG_AND(Cond, ...)        RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #undef RAWR_CLANG_PRAGMA
    #define RAWR_CLANG_PRAGMA(...)           RAWR_RAW_PRAGMA(__VA_ARGS__)
#else
    #undef RAWR_CLANG
    #define RAWR_CLANG(...)
    #undef RAWR_NOT_CLANG
    #define RAWR_NOT_CLANG(...)              __VA_ARGS__
    #undef RAWR_CLANG_ELSE
    #define RAWR_CLANG_ELSE(Clang, NotClang) NotClang
    #undef RAWR_CLANG_AND
    #define RAWR_CLANG_AND(Cond, ...)
    #undef RAWR_CLANG_PRAGMA
    #define RAWR_CLANG_PRAGMA(...)
#endif

#if RAWR_COMPILER_GCC
    #undef RAWR_GCC
    #define RAWR_GCC(...)              __VA_ARGS__
    #undef RAWR_NOT_GCC
    #define RAWR_NOT_GCC(...)
    #undef RAWR_GCC_ELSE
    #define RAWR_GCC_ELSE(GCC, NotGCC) GCC
    #undef RAWR_GCC_AND
    #define RAWR_GCC_AND(Cond, ...)    RAWR_PP_WHEN(Cond, __VA_ARGS__)
    #undef RAWR_GCC_PRAGMA
    #define RAWR_GCC_PRAGMA(...)       RAWR_RAW_PRAGMA(__VA_ARGS__)
#else
    #undef RAWR_GCC
    #define RAWR_GCC(...)
    #undef RAWR_NOT_GCC
    #define RAWR_NOT_GCC(...)          __VA_ARGS__
    #undef RAWR_GCC_ELSE
    #define RAWR_GCC_ELSE(GCC, NotGCC) NotGCC
    #undef RAWR_GCC_AND
    #define RAWR_GCC_AND(Cond, ...)
    #undef RAWR_GCC_PRAGMA
    #define RAWR_GCC_PRAGMA(...)
#endif
