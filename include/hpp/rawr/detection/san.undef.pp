// GENERATED — do not edit
#if RAWR_SAN_ASAN   || RAWR_SAN_HWASAN || RAWR_SAN_TSAN   || RAWR_SAN_MSAN   || RAWR_SAN_LSAN   || RAWR_SAN_UBSAN
    #undef RAWR_SAN_ANY_SANITIZER
#endif
#undef RAWR_SAN_ANY_SANITIZER
#if defined(__MSVC_RUNTIME_CHECKS)
    #undef RAWR_SAN_RTC
#else
    #undef RAWR_SAN_RTC
#endif
#if defined(__has_feature)
    #if __has_feature(safe_stack)
        #undef RAWR_SAN_SAFESTACK
    #else
        #undef RAWR_SAN_SAFESTACK
    #endif
#else
    #undef RAWR_SAN_SAFESTACK
#endif
#if defined(_CONTROL_FLOW_GUARD)
    #undef RAWR_SAN_CFI
#elif defined(__has_feature)
    #if __has_feature(control_flow_integrity)
        #undef RAWR_SAN_CFI
    #else
        #undef RAWR_SAN_CFI
    #endif
#else
    #undef RAWR_SAN_CFI
#endif
#if defined(__SANITIZE_UNDEFINED__)
    #undef RAWR_SAN_UBSAN
#elif defined(__has_feature)
    #if __has_feature(undefined_behavior_sanitizer)
        #undef RAWR_SAN_UBSAN
    #else
        #undef RAWR_SAN_UBSAN
    #endif
#else
    #undef RAWR_SAN_UBSAN
#endif
#if defined(__SANITIZE_LEAK__)
    #undef RAWR_SAN_LSAN
#elif defined(__has_feature)
    #if __has_feature(leak_sanitizer)
        #undef RAWR_SAN_LSAN
    #else
        #undef RAWR_SAN_LSAN
    #endif
#else
    #undef RAWR_SAN_LSAN
#endif
#if defined(__SANITIZE_MEMORY__)
    #undef RAWR_SAN_MSAN
#elif defined(__has_feature)
    #if __has_feature(memory_sanitizer)
        #undef RAWR_SAN_MSAN
    #else
        #undef RAWR_SAN_MSAN
    #endif
#else
    #undef RAWR_SAN_MSAN
#endif
#if defined(__SANITIZE_THREAD__)
    #undef RAWR_SAN_TSAN
#elif defined(__has_feature)
    #if __has_feature(thread_sanitizer)
        #undef RAWR_SAN_TSAN
    #else
        #undef RAWR_SAN_TSAN
    #endif
#else
    #undef RAWR_SAN_TSAN
#endif
#if defined(__SANITIZE_HWADDRESS__)
    #undef RAWR_SAN_HWASAN
#elif defined(__has_feature)
    #if __has_feature(hwaddress_sanitizer)
        #undef RAWR_SAN_HWASAN
    #else
        #undef RAWR_SAN_HWASAN
    #endif
#else
    #undef RAWR_SAN_HWASAN
#endif
#if defined(__SANITIZE_ADDRESS__)
    #undef RAWR_SAN_ASAN
#elif defined(__has_feature)
    #if __has_feature(address_sanitizer)
        #undef RAWR_SAN_ASAN
    #else
        #undef RAWR_SAN_ASAN
    #endif
#else
    #undef RAWR_SAN_ASAN
#endif
#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
#endif
