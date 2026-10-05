#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.detection.cxx_abi.pp
#endif

#include "rawr/detection/compiler.pp"

// Determined by compiler and target. Describes what the compiler
// emits for __cxa_atexit, guards, vtables, RTTI, exception tables.
#define RAWR_CXX_ABI_ITANIUM 0  // GCC, Clang (non-MSVC target)
#define RAWR_CXX_ABI_MSVC    0  // MSVC, clang-cl
#define RAWR_CXX_ABI_UNKNOWN 0
// _MSC_VER is defined by both MSVC and clang-cl (Clang targeting MSVC ABI).
// That is the correct discriminator — it's about the target ABI, not the compiler.
#if defined(_MSC_VER)
    #undef  RAWR_CXX_ABI_MSVC
    #define RAWR_CXX_ABI_MSVC 1
#elif RAWR_COMPILER_GCC || RAWR_COMPILER_CLANG
    #undef  RAWR_CXX_ABI_ITANIUM
    #define RAWR_CXX_ABI_ITANIUM 1
#else
    #undef  RAWR_CXX_ABI_UNKNOWN
    #define RAWR_CXX_ABI_UNKNOWN 1
#endif
