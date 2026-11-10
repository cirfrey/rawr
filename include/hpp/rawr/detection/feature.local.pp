// GENERATED — do not edit

#ifdef RAWR_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.detection.feature.pp
#endif

#include "rawr/detection/compiler.local.pp"

#if defined(__SIZEOF_INT128__)
    #undef RAWR_INTEGER_HAS_INT128
    #define RAWR_INTEGER_HAS_INT128 1
#else
    #undef RAWR_INTEGER_HAS_INT128
    #define RAWR_INTEGER_HAS_INT128 0
#endif

#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
    #undef RAWR_CXX_HAS_EXCEPTIONS
    #define RAWR_CXX_HAS_EXCEPTIONS 1
#else
    #undef RAWR_CXX_HAS_EXCEPTIONS
    #define RAWR_CXX_HAS_EXCEPTIONS 0
#endif

#if RAWR_COMPILER_GNU && defined(__GCC_HAVE_DWARF2_CFI_ASM)
    #undef RAWR_CXX_HAS_CFI_ASM
    #define RAWR_CXX_HAS_CFI_ASM 1
#else
    #undef RAWR_CXX_HAS_CFI_ASM
    #define RAWR_CXX_HAS_CFI_ASM 0
#endif

#if defined(__cpp_rtti) || defined(__GXX_RTTI) || defined(_CPPRTTI)
    #undef RAWR_CXX_HAS_RTTI
    #define RAWR_CXX_HAS_RTTI 1
#else
    #undef RAWR_CXX_HAS_RTTI
    #define RAWR_CXX_HAS_RTTI 0
#endif
