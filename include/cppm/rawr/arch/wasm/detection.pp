#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.arch.wasm.detection.pp
#endif

#include "rawr/arch/detection.pp"

// --- Ids. ---

#define RAWR_ARCH_WASMFEATUREID_ATOMICS 1
#define RAWR_ARCH_WASMFEATUREID_ALL     1

// --- Detection. ---

#define RAWR_DETECTED_ARCH_WASMFEATURE_ATOMICS 0
#if RAWR_ARCH_WASM
    // Set by Clang when compiled with -matomics (the threads/atomics
    // proposal). Without it, a wasm module has no shared memory across
    // execution agents — see RAWR_ARCH_HAS_CAS below.
    #if defined(__wasm_atomics__)
        #undef  RAWR_DETECTED_ARCH_WASMFEATURE_ATOMICS
        #define RAWR_DETECTED_ARCH_WASMFEATURE_ATOMICS 1
    #endif
#endif

// --- Detected aggregate. ---

#define RAWR_DETECTED_ARCH_WASMFEATURES ( \
    (RAWR_DETECTED_ARCH_WASMFEATURE_ATOMICS * RAWR_ARCH_WASMFEATUREID_ATOMICS) | \
0 )

// --- User override. ---

// -DRAWR_SET_ARCH_WASMFEATURES=(RAWR_ARCH_WASMFEATURE_A|RAWR_ARCH_WASMFEATURE_B) replaces detection (RAWR_DETECTED_ARCH_WASMFEATURES keeps the detected set).
#ifdef RAWR_ARCH_WASMFEATURES
    #error "rawr: RAWR_ARCH_WASMFEATURES is an output of detection; to override it define RAWR_SET_ARCH_WASMFEATURES"
#endif
#ifdef RAWR_SET_ARCH_WASMFEATURES
    #if RAWR_SET_ARCH_WASMFEATURES & ~RAWR_ARCH_WASMFEATUREID_ALL
        #error "rawr: RAWR_SET_ARCH_WASMFEATURES contains bits that are not RAWR_ARCH_WASMFEATUREID_* values"
    #endif
    #define RAWR_ARCH_WASMFEATURES           RAWR_SET_ARCH_WASMFEATURES
    #define RAWR_ARCH_WASMFEATURES_DETECTED  0
    #define RAWR_ARCH_WASMFEATURES_OVERRIDEN 1
#else
    #define RAWR_ARCH_WASMFEATURES           RAWR_DETECTED_ARCH_WASMFEATURES
    #define RAWR_ARCH_WASMFEATURES_DETECTED  1
    #define RAWR_ARCH_WASMFEATURES_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#define RAWR_ARCH_WASM_ATOMICS 0
#if RAWR_ARCH_WASMFEATURES & RAWR_ARCH_WASMFEATUREID_ATOMICS
    #undef  RAWR_ARCH_WASM_ATOMICS
    #define RAWR_ARCH_WASM_ATOMICS 1
#endif
