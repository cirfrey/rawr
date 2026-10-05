// GENERATED — do not edit
#if RAWR_COMPILER_GCC
    #undef RAWR_GCC_PRAGMA
    #undef RAWR_GCC_AND
    #undef RAWR_GCC_ELSE
    #undef RAWR_NOT_GCC
    #undef RAWR_GCC
#else
    #undef RAWR_GCC_PRAGMA
    #undef RAWR_GCC_AND
    #undef RAWR_GCC_ELSE
    #undef RAWR_NOT_GCC
    #undef RAWR_GCC
#endif
#if RAWR_COMPILER_CLANG
    #undef RAWR_CLANG_PRAGMA
    #undef RAWR_CLANG_AND
    #undef RAWR_CLANG_ELSE
    #undef RAWR_NOT_CLANG
    #undef RAWR_CLANG
#else
    #undef RAWR_CLANG_PRAGMA
    #undef RAWR_CLANG_AND
    #undef RAWR_CLANG_ELSE
    #undef RAWR_NOT_CLANG
    #undef RAWR_CLANG
#endif
#if RAWR_COMPILER_FAMILY_GNU
    #undef RAWR_GNU_PRAGMA
    #undef RAWR_GNU_AND
    #undef RAWR_GNU_ELSE
    #undef RAWR_NOT_GNU
    #undef RAWR_GNU
#else
    #undef RAWR_GNU_PRAGMA
    #undef RAWR_GNU_AND
    #undef RAWR_GNU_ELSE
    #undef RAWR_NOT_GNU
    #undef RAWR_GNU
#endif
#if RAWR_COMPILER_MSVC
    #undef RAWR_MSVC_INTRIN
    #undef RAWR_MSVC_PRAGMA
    #undef RAWR_MSVC_AND
    #undef RAWR_MSVC_ELSE
    #undef RAWR_NOT_MSVC
    #undef RAWR_MSVC
#else
    #undef RAWR_MSVC_INTRIN
    #undef RAWR_MSVC_PRAGMA
    #undef RAWR_MSVC_AND
    #undef RAWR_MSVC_ELSE
    #undef RAWR_NOT_MSVC
    #undef RAWR_MSVC
#endif
#if RAWR_COMPILER_GCC || (RAWR_COMPILER_CLANG && RAWR_COMPILER_VERSION_MAJOR >= 14)
    #undef RAWR_ATTR_ERROR
#else
    #undef RAWR_ATTR_ERROR
#endif
#if RAWR_COMPILER_CLANG
    #undef RAWR_ASSUME
#elif RAWR_COMPILER_GCC
    #undef RAWR_ASSUME
#elif RAWR_COMPILER_MSVC
    #undef RAWR_ASSUME
#endif
#if RAWR_COMPILER_MSVC
    #undef RAWR_ALTERNATENAME
    #undef RAWR_ASMV
    #undef RAWR_ASM
    #undef RAWR_WEAK
    #undef RAWR_NAKED
    #undef RAWR_FLATTEN
    #undef RAWR_ALWAYS_INLINE
    #undef RAWR_HIDDEN
    #undef RAWR_NORETURN
    #undef RAWR_UNREACHABLE
    #undef RAWR_PRAGMA
    #undef RAWR_ATTRIBUTE
    #undef RAWR_DECLSPEC
#else
    #undef RAWR_ALTERNATENAME
    #undef RAWR_ASMV
    #undef RAWR_ASM
    #undef RAWR_WEAK
    #undef RAWR_NAKED
    #undef RAWR_FLATTEN
    #undef RAWR_ALWAYS_INLINE
    #undef RAWR_HIDDEN
    #undef RAWR_NORETURN
    #undef RAWR_UNREACHABLE
    #undef RAWR_PRAGMA
    #undef RAWR_ATTRIBUTE
    #undef RAWR_DECLSPEC
#endif
#undef RAWR_RAW_ATTRIBUTE
#undef RAWR_RAW_DECLSPEC
#undef RAWR_RAW_PRAGMA
#include "rawr/lib/pp.undef.pp"
#include "rawr/detection/compiler.undef.pp"
#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
#endif
