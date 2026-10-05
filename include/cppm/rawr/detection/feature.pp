#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.detection.feature.pp
#endif

#include "rawr/detection/compiler.pp"

#if defined(__SIZEOF_INT128__)
    #define RAWR_HAS_INT128 1
#else
    #define RAWR_HAS_INT128 0
#endif

#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
    #define RAWR_HAS_EXCEPTIONS 1
#else
    #define RAWR_HAS_EXCEPTIONS 0
#endif

#if RAWR_COMPILER_FAMILY_GNU && defined(__GCC_HAVE_DWARF2_CFI_ASM)
    #define RAWR_HAS_CFI_ASM 1
#else
    #define RAWR_HAS_CFI_ASM 0
#endif

#if defined(__cpp_rtti) || defined(__GXX_RTTI) || defined(_CPPRTTI)
    #define RAWR_HAS_RTTI 1
#else
    #define RAWR_HAS_RTTI 0
#endif
