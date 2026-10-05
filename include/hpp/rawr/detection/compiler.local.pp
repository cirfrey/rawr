// GENERATED — do not edit

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.detection.compiler.pp
#endif

#undef RAWR_DETECTION_MIN_GCC_VERSION
#define RAWR_DETECTION_MIN_GCC_VERSION   11
#undef RAWR_DETECTION_MIN_CLANG_VERSION
#define RAWR_DETECTION_MIN_CLANG_VERSION 12
#undef RAWR_DETECTION_MIN_MSVC_VERSION
#define RAWR_DETECTION_MIN_MSVC_VERSION  1928

#undef RAWR_COMPILER_CLANG
#define RAWR_COMPILER_CLANG   0
#undef RAWR_COMPILER_GCC
#define RAWR_COMPILER_GCC     0
#undef RAWR_COMPILER_MSVC
#define RAWR_COMPILER_MSVC    0
#undef RAWR_COMPILER_UNKNOWN
#define RAWR_COMPILER_UNKNOWN 0
// Clang must come before GCC — clang-cl defines both __clang__ and _MSC_VER,
// and Apple Clang defines __GNUC__ for compatibility.
#if defined(__clang__)
    #undef  RAWR_COMPILER_CLANG
    #define RAWR_COMPILER_CLANG        1
    #undef RAWR_COMPILER_VERSION
    #define RAWR_COMPILER_VERSION      (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
    #undef RAWR_COMPILER_VERSION_MAJOR
    #define RAWR_COMPILER_VERSION_MAJOR __clang_major__
    #undef RAWR_COMPILER_VERSION_MINOR
    #define RAWR_COMPILER_VERSION_MINOR __clang_minor__
    #undef RAWR_COMPILER_VERSION_PATCH
    #define RAWR_COMPILER_VERSION_PATCH __clang_patchlevel__
    #undef RAWR_COMPILER_VERSION_BUILD
    #define RAWR_COMPILER_VERSION_BUILD 0
#elif defined(__GNUC__) || defined(__GNUG__)
    #undef  RAWR_COMPILER_GCC
    #define RAWR_COMPILER_GCC           1
    #undef RAWR_COMPILER_VERSION
    #define RAWR_COMPILER_VERSION       (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
    #undef RAWR_COMPILER_VERSION_MAJOR
    #define RAWR_COMPILER_VERSION_MAJOR __GNUC__
    #undef RAWR_COMPILER_VERSION_MINOR
    #define RAWR_COMPILER_VERSION_MINOR __GNUC_MINOR__
    #undef RAWR_COMPILER_VERSION_PATCH
    #define RAWR_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
    #undef RAWR_COMPILER_VERSION_BUILD
    #define RAWR_COMPILER_VERSION_BUILD 0
#elif defined(_MSC_VER)
    #undef  RAWR_COMPILER_MSVC
    #define RAWR_COMPILER_MSVC          1
    #undef RAWR_COMPILER_VERSION
    #define RAWR_COMPILER_VERSION       _MSC_FULL_VER
    #undef RAWR_COMPILER_VERSION_MAJOR
    #define RAWR_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
    #undef RAWR_COMPILER_VERSION_MINOR
    #define RAWR_COMPILER_VERSION_MINOR (_MSC_VER % 100)
    #undef RAWR_COMPILER_VERSION_PATCH
    #define RAWR_COMPILER_VERSION_PATCH 0
    #undef RAWR_COMPILER_VERSION_BUILD
    #define RAWR_COMPILER_VERSION_BUILD (_MSC_FULL_VER % 100000)
#else
    #undef  RAWR_COMPILER_UNKNOWN
    #define RAWR_COMPILER_UNKNOWN       1
    #undef RAWR_COMPILER_VERSION
    #define RAWR_COMPILER_VERSION       0
    #undef RAWR_COMPILER_VERSION_MAJOR
    #define RAWR_COMPILER_VERSION_MAJOR 0
    #undef RAWR_COMPILER_VERSION_MINOR
    #define RAWR_COMPILER_VERSION_MINOR 0
    #undef RAWR_COMPILER_VERSION_PATCH
    #define RAWR_COMPILER_VERSION_PATCH 0
    #undef RAWR_COMPILER_VERSION_BUILD
    #define RAWR_COMPILER_VERSION_BUILD 0
#endif

#undef RAWR_COMPILER_FAMILY_GNU
#define RAWR_COMPILER_FAMILY_GNU 0
#if RAWR_COMPILER_CLANG || RAWR_COMPILER_GCC
    #undef  RAWR_COMPILER_FAMILY_GNU
    #define RAWR_COMPILER_FAMILY_GNU 1
#endif

#if !RAWR_DETECTION_NO_COMPILER_ERROR
    #if RAWR_COMPILER_GCC
        #if RAWR_COMPILER_VERSION_MAJOR < RAWR_DETECTION_MIN_GCC_VERSION
            #error "rawr/detection/compiler.pp: GCC 11+ required (concepts correctness, NTTP structural types)"
        #endif
    #elif RAWR_COMPILER_CLANG
        #if RAWR_COMPILER_VERSION_MAJOR < RAWR_DETECTION_MIN_CLANG_VERSION
            #error "rawr/detection/compiler.pp: Clang 12+ required (concepts constraint subsumption correctness)"
        #endif
    #elif RAWR_COMPILER_MSVC
        #if _MSC_VER < RAWR_DETECTION_MIN_MSVC_VERSION
            #error "rawr/detection/compiler.pp: MSVC 16.8+ (_MSC_VER >= 1928) required (C++20 mode, concepts)"
        #endif
    #else
        #error "rawr/detection/compiler.pp: unrecognized compiler"
    #endif
#endif
