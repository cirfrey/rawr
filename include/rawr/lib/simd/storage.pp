//// rawr/lib/simd/storage.pp.
#pragma once

#include "rawr/lib/detection.pp"

#if RAWR_COMPILER_FAMILY_GNU
    // By wrapping the vector attribute in a struct, we completely disable
    // the implicit GNU math operators (+, -, *, /) making the types inert.
    // The x64 System V and ARM64 ABIs guarantee this is still passed in
    // SIMD registers with zero overhead.
    #define RAWR_LIB_SIMD_STORAGE(Type, Lanes)                 \
        struct Type##x##Lanes {                                \
            using value_type = Type;                           \
            static constexpr auto lane_count = Lanes;          \
            value_type                                         \
            __attribute__((vector_size(sizeof(Type) * Lanes))) \
                lanes;                                         \
        }
#elif RAWR_COMPILER_MSVC
    #define RAWR_LIB_SIMD_STORAGE(Type, Lanes)        \
        struct                                        \
        __declspec(intrin_type)                       \
        alignas(sizeof(Type) * Lanes)                 \
        Type##x##Lanes {                              \
            using value_type = Type;                  \
            static constexpr auto lane_count = Lanes; \
            value_type lanes[Lanes];                  \
        }
#endif
