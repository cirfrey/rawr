//// rawr/san/attributes.pp.
#pragma once

#include "rawr/lib/detection.pp"

// Gnu specific (clang/gcc).
#if RAWR_COMPILER_FAMILY_GNU
    #define RAWR_NO_SANITIZE_THREAD    __attribute__((no_sanitize("thread")))
    #define RAWR_NO_SANITIZE_MEMORY    __attribute__((no_sanitize("memory")))
    #define RAWR_NO_SANITIZE_UNDEFINED __attribute__((no_sanitize("undefined")))
    #define RAWR_NO_SANITIZE_HWADDRESS __attribute__((no_sanitize("hwaddress")))
#else
    #define RAWR_NO_SANITIZE_THREAD
    #define RAWR_NO_SANITIZE_MEMORY
    #define RAWR_NO_SANITIZE_UNDEFINED
    #define RAWR_NO_SANITIZE_HWADDRESS
#endif

// Common.
#if RAWR_COMPILER_FAMILY_GNU
    #define RAWR_NO_SANITIZE_ADDRESS __attribute__((no_sanitize("address")))
    #define RAWR_NO_SANITIZE_CFI     __attribute__((no_sanitize("cfi")))
#else
    #define RAWR_NO_SANITIZE_ADDRESS __declspec(no_sanitize_address)
    #define RAWR_NO_SANITIZE_CFI     __declspec(guard(nocf))
#endif
