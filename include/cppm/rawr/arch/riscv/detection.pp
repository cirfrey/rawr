#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.arch.riscv.detection.pp
#endif

#include "rawr/arch/detection.pp"

// --- Ids. ---

#define RAWR_ARCH_RISCVFEATUREID_ATOMIC 1
#define RAWR_ARCH_RISCVFEATUREID_ALL    1

// --- Detection. ---

#define RAWR_DETECTED_ARCH_RISCVFEATURE_ATOMIC 0
#if RAWR_ARCH_RISCV
    // Set by GCC/Clang when the 'A' (atomic) extension is targeted
    // (-matomic, or an "a"-suffixed -march, e.g. rv32ima). Many minimal
    // embedded RV32I cores omit the A extension deliberately to save area.
    #if defined(__riscv_atomic)
        #undef  RAWR_DETECTED_ARCH_RISCVFEATURE_ATOMIC
        #define RAWR_DETECTED_ARCH_RISCVFEATURE_ATOMIC 1
    #endif
#endif

// --- Detected aggregate. ---

#define RAWR_DETECTED_ARCH_RISCVFEATURES ( \
    (RAWR_DETECTED_ARCH_RISCVFEATURE_ATOMIC * RAWR_ARCH_RISCVFEATUREID_ATOMIC) | \
0 )

// --- User override. ---
// -DRAWR_SET_ARCH_RISCVFEATURES=(RAWR_ARCH_RISCVFEATURE_A|RAWR_ARCH_RISCVFEATURE_B) replaces detection (RAWR_DETECTED_ARCH_RISCVFEATURES keeps the detected set).
#ifdef RAWR_ARCH_RISCVFEATURES
    #error "rawr: RAWR_ARCH_RISCVFEATURES is an output of detection; to override it define RAWR_SET_ARCH_RISCVFEATURES"
#endif
#ifdef RAWR_SET_ARCH_RISCVFEATURES
    #if RAWR_SET_ARCH_RISCVFEATURES & ~RAWR_ARCH_RISCVFEATUREID_ALL
        #error "rawr: RAWR_SET_ARCH_RISCVFEATURES contains bits that are not RAWR_ARCH_RISCVFEATUREID_* values"
    #endif
    #define RAWR_ARCH_RISCVFEATURES           RAWR_SET_ARCH_RISCVFEATURES
    #define RAWR_ARCH_RISCVFEATURES_DETECTED  0
    #define RAWR_ARCH_RISCVFEATURES_OVERRIDEN 1
#else
    #define RAWR_ARCH_RISCVFEATURES           RAWR_DETECTED_ARCH_RISCVFEATURES
    #define RAWR_ARCH_RISCVFEATURES_DETECTED  1
    #define RAWR_ARCH_RISCVFEATURES_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#define RAWR_ARCH_RISCV_ATOMIC 0
#if RAWR_ARCH_RISCVFEATURES & RAWR_ARCH_RISCVFEATUREID_ATOMIC
    #undef  RAWR_ARCH_RISCV_ATOMIC
    #define RAWR_ARCH_RISCV_ATOMIC 1
#endif
