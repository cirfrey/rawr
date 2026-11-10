#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.san.detection.pp
#endif

// --- Ids. ---

#define RAWR_SANFEATUREID_ASAN      1
#define RAWR_SANFEATUREID_HWASAN    2
#define RAWR_SANFEATUREID_TSAN      4
#define RAWR_SANFEATUREID_MSAN      8
#define RAWR_SANFEATUREID_LSAN      16
#define RAWR_SANFEATUREID_UBSAN     32
#define RAWR_SANFEATUREID_CFI       64
#define RAWR_SANFEATUREID_SAFESTACK 128
#define RAWR_SANFEATUREID_RTC       256
#define RAWR_SANFEATUREID_ALL       511

// --- Detection. ---

// Guarding __has_feature inside an #elif chain prevents MSVC from
// aggressively expanding it and throwing C1012.

// --- Address Sanitizer (ASAN) ---
#if defined(__SANITIZE_ADDRESS__)
    #define RAWR_DETECTED_SAN_ASAN 1
#elif defined(__has_feature)
    #if __has_feature(address_sanitizer)
        #define RAWR_DETECTED_SAN_ASAN 1
    #else
        #define RAWR_DETECTED_SAN_ASAN 0
    #endif
#else
    #define RAWR_DETECTED_SAN_ASAN 0
#endif

// --- Hardware-Assisted Address Sanitizer (HWASAN) ---
#if defined(__SANITIZE_HWADDRESS__)
    #define RAWR_DETECTED_SAN_HWASAN 1
#elif defined(__has_feature)
    #if __has_feature(hwaddress_sanitizer)
        #define RAWR_DETECTED_SAN_HWASAN 1
    #else
        #define RAWR_DETECTED_SAN_HWASAN 0
    #endif
#else
    #define RAWR_DETECTED_SAN_HWASAN 0
#endif

// --- Thread Sanitizer (TSAN) ---
#if defined(__SANITIZE_THREAD__)
    #define RAWR_DETECTED_SAN_TSAN 1
#elif defined(__has_feature)
    #if __has_feature(thread_sanitizer)
        #define RAWR_DETECTED_SAN_TSAN 1
    #else
        #define RAWR_DETECTED_SAN_TSAN 0
    #endif
#else
    #define RAWR_DETECTED_SAN_TSAN 0
#endif

// --- Memory Sanitizer (MSAN) ---
#if defined(__SANITIZE_MEMORY__)
    #define RAWR_DETECTED_SAN_MSAN 1
#elif defined(__has_feature)
    #if __has_feature(memory_sanitizer)
        #define RAWR_DETECTED_SAN_MSAN 1
    #else
        #define RAWR_DETECTED_SAN_MSAN 0
    #endif
#else
    #define RAWR_DETECTED_SAN_MSAN 0
#endif

// --- Leak Sanitizer (LSAN) ---
#if defined(__SANITIZE_LEAK__)
    #define RAWR_DETECTED_SAN_LSAN 1
#elif defined(__has_feature)
    #if __has_feature(leak_sanitizer)
        #define RAWR_DETECTED_SAN_LSAN 1
    #else
        #define RAWR_DETECTED_SAN_LSAN 0
    #endif
#else
    #define RAWR_DETECTED_SAN_LSAN 0
#endif

// --- Undefined Behavior Sanitizer (UBSAN) ---
#if defined(__SANITIZE_UNDEFINED__)
    #define RAWR_DETECTED_SAN_UBSAN 1
#elif defined(__has_feature)
    #if __has_feature(undefined_behavior_sanitizer)
        #define RAWR_DETECTED_SAN_UBSAN 1
    #else
        #define RAWR_DETECTED_SAN_UBSAN 0
    #endif
#else
    #define RAWR_DETECTED_SAN_UBSAN 0
#endif

// --- Control Flow Guard / Integrity (CFG / CFI) ---
#if defined(_CONTROL_FLOW_GUARD)
    #define RAWR_DETECTED_SAN_CFI 1 // MSVC CFG
#elif defined(__has_feature)
    #if __has_feature(control_flow_integrity)
        #define RAWR_DETECTED_SAN_CFI 1 // Clang CFI
    #else
        #define RAWR_DETECTED_SAN_CFI 0
    #endif
#else
    #define RAWR_DETECTED_SAN_CFI 0
#endif

// --- SafeStack ---
#if defined(__has_feature)
    #if __has_feature(safe_stack)
        #define RAWR_DETECTED_SAN_SAFESTACK 1
    #else
        #define RAWR_DETECTED_SAN_SAFESTACK 0
    #endif
#else
    #define RAWR_DETECTED_SAN_SAFESTACK 0
#endif

// --- MSVC Runtime Checks (/RTC) ---
#if defined(__MSVC_RUNTIME_CHECKS)
    #define RAWR_DETECTED_SAN_RTC 1
#else
    #define RAWR_DETECTED_SAN_RTC 0
#endif


// --- Detected aggregate. ---

#define RAWR_DETECTED_SANFEATURES ( \
    (RAWR_DETECTED_SAN_ASAN      * RAWR_SANFEATUREID_ASAN) | \
    (RAWR_DETECTED_SAN_HWASAN    * RAWR_SANFEATUREID_HWASAN) | \
    (RAWR_DETECTED_SAN_TSAN      * RAWR_SANFEATUREID_TSAN) | \
    (RAWR_DETECTED_SAN_MSAN      * RAWR_SANFEATUREID_MSAN) | \
    (RAWR_DETECTED_SAN_LSAN      * RAWR_SANFEATUREID_LSAN) | \
    (RAWR_DETECTED_SAN_UBSAN     * RAWR_SANFEATUREID_UBSAN) | \
    (RAWR_DETECTED_SAN_CFI       * RAWR_SANFEATUREID_CFI) | \
    (RAWR_DETECTED_SAN_SAFESTACK * RAWR_SANFEATUREID_SAFESTACK) | \
    (RAWR_DETECTED_SAN_RTC       * RAWR_SANFEATUREID_RTC) | \
0 )

// --- User override. ---
// -DRAWR_SET_SANFEATURES=(RAWR_SANFEATURE_A|RAWR_SANFEATURE_B) replaces detection (RAWR_DETECTED_SANFEATURES keeps the detected set).
#ifdef RAWR_SANFEATURES
    #error "rawr: RAWR_SANFEATURES is an output of detection; to override it define RAWR_SET_SANFEATURES"
#endif
#ifdef RAWR_SET_SANFEATURES
    #if RAWR_SET_SANFEATURES & ~RAWR_SANFEATUREID_ALL
        #error "rawr: RAWR_SET_SANFEATURES contains bits that are not RAWR_SANFEATUREID_* values"
    #endif
    #define RAWR_SANFEATURES           RAWR_SET_SANFEATURES
    #define RAWR_SANFEATURES_DETECTED  0
    #define RAWR_SANFEATURES_OVERRIDEN 1
#else
    #define RAWR_SANFEATURES           RAWR_DETECTED_SANFEATURES
    #define RAWR_SANFEATURES_DETECTED  1
    #define RAWR_SANFEATURES_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#define RAWR_SANFEATURE_ASAN 0
#define RAWR_SANFEATURE_HWASAN 0
#define RAWR_SANFEATURE_TSAN 0
#define RAWR_SANFEATURE_MSAN 0
#define RAWR_SANFEATURE_LSAN 0
#define RAWR_SANFEATURE_UBSAN 0
#define RAWR_SANFEATURE_CFI 0
#define RAWR_SANFEATURE_SAFESTACK 0
#define RAWR_SANFEATURE_RTC 0
#if RAWR_SANFEATURES & RAWR_SANFEATURE_ASAN
    #undef  RAWR_SANFEATURE_ASAN
    #define RAWR_SANFEATURE_ASAN 1
#endif
#if RAWR_SANFEATURES & RAWR_SANFEATURE_HWASAN
    #undef  RAWR_SANFEATURE_HWASAN
    #define RAWR_SANFEATURE_HWASAN 1
#endif
#if RAWR_SANFEATURES & RAWR_SANFEATURE_TSAN
    #undef  RAWR_SANFEATURE_TSAN
    #define RAWR_SANFEATURE_TSAN 1
#endif
#if RAWR_SANFEATURES & RAWR_SANFEATURE_MSAN
    #undef  RAWR_SANFEATURE_MSAN
    #define RAWR_SANFEATURE_MSAN 1
#endif
#if RAWR_SANFEATURES & RAWR_SANFEATURE_LSAN
    #undef  RAWR_SANFEATURE_LSAN
    #define RAWR_SANFEATURE_LSAN 1
#endif
#if RAWR_SANFEATURES & RAWR_SANFEATURE_UBSAN
    #undef  RAWR_SANFEATURE_UBSAN
    #define RAWR_SANFEATURE_UBSAN 1
#endif
#if RAWR_SANFEATURES & RAWR_SANFEATURE_CFI
    #undef  RAWR_SANFEATURE_CFI
    #define RAWR_SANFEATURE_CFI 1
#endif
#if RAWR_SANFEATURES & RAWR_SANFEATURE_SAFESTACK
    #undef  RAWR_SANFEATURE_SAFESTACK
    #define RAWR_SANFEATURE_SAFESTACK 1
#endif
#if RAWR_SANFEATURES & RAWR_SANFEATURE_RTC
    #undef  RAWR_SANFEATURE_RTC
    #define RAWR_SANFEATURE_RTC 1
#endif
