// GENERATED — do not edit

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.detection.san.pp
#endif

// Guarding __has_feature inside an #elif chain prevents MSVC from
// aggressively expanding it and throwing C1012.

// --- Address Sanitizer (ASAN) ---
#if defined(__SANITIZE_ADDRESS__)
    #undef RAWR_SAN_ASAN
    #define RAWR_SAN_ASAN 1
#elif defined(__has_feature)
    #if __has_feature(address_sanitizer)
        #undef RAWR_SAN_ASAN
        #define RAWR_SAN_ASAN 1
    #else
        #undef RAWR_SAN_ASAN
        #define RAWR_SAN_ASAN 0
    #endif
#else
    #undef RAWR_SAN_ASAN
    #define RAWR_SAN_ASAN 0
#endif

// --- Hardware-Assisted Address Sanitizer (HWASAN) ---
#if defined(__SANITIZE_HWADDRESS__)
    #undef RAWR_SAN_HWASAN
    #define RAWR_SAN_HWASAN 1
#elif defined(__has_feature)
    #if __has_feature(hwaddress_sanitizer)
        #undef RAWR_SAN_HWASAN
        #define RAWR_SAN_HWASAN 1
    #else
        #undef RAWR_SAN_HWASAN
        #define RAWR_SAN_HWASAN 0
    #endif
#else
    #undef RAWR_SAN_HWASAN
    #define RAWR_SAN_HWASAN 0
#endif

// --- Thread Sanitizer (TSAN) ---
#if defined(__SANITIZE_THREAD__)
    #undef RAWR_SAN_TSAN
    #define RAWR_SAN_TSAN 1
#elif defined(__has_feature)
    #if __has_feature(thread_sanitizer)
        #undef RAWR_SAN_TSAN
        #define RAWR_SAN_TSAN 1
    #else
        #undef RAWR_SAN_TSAN
        #define RAWR_SAN_TSAN 0
    #endif
#else
    #undef RAWR_SAN_TSAN
    #define RAWR_SAN_TSAN 0
#endif

// --- Memory Sanitizer (MSAN) ---
#if defined(__SANITIZE_MEMORY__)
    #undef RAWR_SAN_MSAN
    #define RAWR_SAN_MSAN 1
#elif defined(__has_feature)
    #if __has_feature(memory_sanitizer)
        #undef RAWR_SAN_MSAN
        #define RAWR_SAN_MSAN 1
    #else
        #undef RAWR_SAN_MSAN
        #define RAWR_SAN_MSAN 0
    #endif
#else
    #undef RAWR_SAN_MSAN
    #define RAWR_SAN_MSAN 0
#endif

// --- Leak Sanitizer (LSAN) ---
#if defined(__SANITIZE_LEAK__)
    #undef RAWR_SAN_LSAN
    #define RAWR_SAN_LSAN 1
#elif defined(__has_feature)
    #if __has_feature(leak_sanitizer)
        #undef RAWR_SAN_LSAN
        #define RAWR_SAN_LSAN 1
    #else
        #undef RAWR_SAN_LSAN
        #define RAWR_SAN_LSAN 0
    #endif
#else
    #undef RAWR_SAN_LSAN
    #define RAWR_SAN_LSAN 0
#endif

// --- Undefined Behavior Sanitizer (UBSAN) ---
#if defined(__SANITIZE_UNDEFINED__)
    #undef RAWR_SAN_UBSAN
    #define RAWR_SAN_UBSAN 1
#elif defined(__has_feature)
    #if __has_feature(undefined_behavior_sanitizer)
        #undef RAWR_SAN_UBSAN
        #define RAWR_SAN_UBSAN 1
    #else
        #undef RAWR_SAN_UBSAN
        #define RAWR_SAN_UBSAN 0
    #endif
#else
    #undef RAWR_SAN_UBSAN
    #define RAWR_SAN_UBSAN 0
#endif

// --- Control Flow Guard / Integrity (CFG / CFI) ---
#if defined(_CONTROL_FLOW_GUARD)
    #undef RAWR_SAN_CFI
    #define RAWR_SAN_CFI 1 // MSVC CFG
#elif defined(__has_feature)
    #if __has_feature(control_flow_integrity)
        #undef RAWR_SAN_CFI
        #define RAWR_SAN_CFI 1 // Clang CFI
    #else
        #undef RAWR_SAN_CFI
        #define RAWR_SAN_CFI 0
    #endif
#else
    #undef RAWR_SAN_CFI
    #define RAWR_SAN_CFI 0
#endif

// --- SafeStack ---
#if defined(__has_feature)
    #if __has_feature(safe_stack)
        #undef RAWR_SAN_SAFESTACK
        #define RAWR_SAN_SAFESTACK 1
    #else
        #undef RAWR_SAN_SAFESTACK
        #define RAWR_SAN_SAFESTACK 0
    #endif
#else
    #undef RAWR_SAN_SAFESTACK
    #define RAWR_SAN_SAFESTACK 0
#endif

// --- MSVC Runtime Checks (/RTC) ---
#if defined(__MSVC_RUNTIME_CHECKS)
    #undef RAWR_SAN_RTC
    #define RAWR_SAN_RTC 1
#else
    #undef RAWR_SAN_RTC
    #define RAWR_SAN_RTC 0
#endif

// Helper for "any sanitizer is active" (useful for tweaking timeouts or disabling optimizations)
// Note: Excludes CFI, SafeStack, and RTC as they usually don't dictate timeout adjustments.
#undef RAWR_SAN_ANY_SANITIZER
#define RAWR_SAN_ANY_SANITIZER 0
#if RAWR_SAN_ASAN   || \
    RAWR_SAN_HWASAN || \
    RAWR_SAN_TSAN   || \
    RAWR_SAN_MSAN   || \
    RAWR_SAN_LSAN   || \
    RAWR_SAN_UBSAN
    #undef  RAWR_SAN_ANY_SANITIZER
    #define RAWR_SAN_ANY_SANITIZER 1
#endif
