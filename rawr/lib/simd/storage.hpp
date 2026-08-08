#pragma region rawr/lib/simd/storage.hpp

#ifndef RAWR_UNITY
    #pragma once
    #include "rawr/lib/detection_macros.hpp"
    #include "rawr/lib/aliases.hpp"
#endif

namespace rawr::inline lib::simd::storage
{
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
        #define RAWR_LIB_SIMD_STORAGE(Type, Lanes)   \
            struct                                        \
            __declspec(intrin_type)                       \
            alignas(sizeof(Type) * Lanes)                 \
            Type##x##Lanes {                              \
                using value_type = Type;                  \
                static constexpr auto lane_count = Lanes; \
                value_type lanes[Lanes];                  \
            }
    #endif

    inline namespace _128b
    {
        RAWR_LIB_SIMD_STORAGE(u8,  16);
        RAWR_LIB_SIMD_STORAGE(s8,  16);
        RAWR_LIB_SIMD_STORAGE(u16, 8);
        RAWR_LIB_SIMD_STORAGE(s16, 8);
        RAWR_LIB_SIMD_STORAGE(u32, 4);
        RAWR_LIB_SIMD_STORAGE(s32, 4);
        RAWR_LIB_SIMD_STORAGE(f32, 4);
        RAWR_LIB_SIMD_STORAGE(u64, 2);
        RAWR_LIB_SIMD_STORAGE(s64, 2);
        RAWR_LIB_SIMD_STORAGE(f64, 2);
    }

    inline namespace _256b
    {
        RAWR_LIB_SIMD_STORAGE(u8,  32);
        RAWR_LIB_SIMD_STORAGE(s8,  32);
        RAWR_LIB_SIMD_STORAGE(u16, 16);
        RAWR_LIB_SIMD_STORAGE(s16, 16);
        RAWR_LIB_SIMD_STORAGE(u32, 8);
        RAWR_LIB_SIMD_STORAGE(s32, 8);
        RAWR_LIB_SIMD_STORAGE(f32, 8);
        RAWR_LIB_SIMD_STORAGE(u64, 4);
        RAWR_LIB_SIMD_STORAGE(s64, 4);
        RAWR_LIB_SIMD_STORAGE(f64, 4);
    }

    inline namespace _512b
    {
        RAWR_LIB_SIMD_STORAGE(u8,  64);
        RAWR_LIB_SIMD_STORAGE(s8,  64);
        RAWR_LIB_SIMD_STORAGE(u16, 32);
        RAWR_LIB_SIMD_STORAGE(s16, 32);
        RAWR_LIB_SIMD_STORAGE(u32, 16);
        RAWR_LIB_SIMD_STORAGE(s32, 16);
        RAWR_LIB_SIMD_STORAGE(f32, 16);
        RAWR_LIB_SIMD_STORAGE(u64, 8);
        RAWR_LIB_SIMD_STORAGE(s64, 8);
        RAWR_LIB_SIMD_STORAGE(f64, 8);
    }
} // namespace rawr::lib::simd

#pragma endregion rawr/lib/simd/storage.hpp
