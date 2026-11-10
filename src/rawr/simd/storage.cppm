export module rawr.simd.storage;
import        rawr.lib.integer.raw;
#include     "rawr/compiler/utils.pp"


// By wrapping the vector attribute in a struct, we completely disable
// the implicit GNU math operators (+, -, *, /) making the types inert.
// The x64 System V and ARM64 ABIs guarantee this is still passed in
// SIMD registers with zero overhead.
#define RAWR_LIB_SIMD_STORAGE(Type, Lanes)                    \
    struct                                                    \
        RAWR_DECLSPEC(intrin_type)                            \
        alignas(sizeof(Type) * Lanes)                         \
    Type##x##Lanes {                                          \
        using value_type = Type;                              \
        static constexpr auto lane_count = Lanes;             \
        value_type                                            \
            RAWR_ATTRIBUTE(vector_size(sizeof(Type) * Lanes)) \
        lanes RAWR_MSVC([Lanes]);                             \
    }

export namespace rawr::simd::storage
{
    inline namespace _128b
    {
        RAWR_LIB_SIMD_STORAGE(ru8,  16);
        RAWR_LIB_SIMD_STORAGE(rs8,  16);
        RAWR_LIB_SIMD_STORAGE(ru16, 8);
        RAWR_LIB_SIMD_STORAGE(rs16, 8);
        RAWR_LIB_SIMD_STORAGE(ru32, 4);
        RAWR_LIB_SIMD_STORAGE(rs32, 4);
        RAWR_LIB_SIMD_STORAGE(rf32, 4);
        RAWR_LIB_SIMD_STORAGE(ru64, 2);
        RAWR_LIB_SIMD_STORAGE(rs64, 2);
        RAWR_LIB_SIMD_STORAGE(rf64, 2);
    }

    inline namespace _256b
    {
        RAWR_LIB_SIMD_STORAGE(ru8,  32);
        RAWR_LIB_SIMD_STORAGE(rs8,  32);
        RAWR_LIB_SIMD_STORAGE(ru16, 16);
        RAWR_LIB_SIMD_STORAGE(rs16, 16);
        RAWR_LIB_SIMD_STORAGE(ru32, 8);
        RAWR_LIB_SIMD_STORAGE(rs32, 8);
        RAWR_LIB_SIMD_STORAGE(rf32, 8);
        RAWR_LIB_SIMD_STORAGE(ru64, 4);
        RAWR_LIB_SIMD_STORAGE(rs64, 4);
        RAWR_LIB_SIMD_STORAGE(rf64, 4);
    }

    inline namespace _512b
    {
        RAWR_LIB_SIMD_STORAGE(ru8,  64);
        RAWR_LIB_SIMD_STORAGE(rs8,  64);
        RAWR_LIB_SIMD_STORAGE(ru16, 32);
        RAWR_LIB_SIMD_STORAGE(rs16, 32);
        RAWR_LIB_SIMD_STORAGE(ru32, 16);
        RAWR_LIB_SIMD_STORAGE(rs32, 16);
        RAWR_LIB_SIMD_STORAGE(rf32, 16);
        RAWR_LIB_SIMD_STORAGE(ru64, 8);
        RAWR_LIB_SIMD_STORAGE(rs64, 8);
        RAWR_LIB_SIMD_STORAGE(rf64, 8);
    }
}
