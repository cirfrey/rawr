export module rawr.lib.simd.storage;
import        rawr.lib.integer.raw;
#include     "rawr/lib/simd/storage.pp"

export namespace rawr::inline lib::simd::storage
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
