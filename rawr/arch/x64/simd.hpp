#pragma region rawr/arch/x64/simd.hpp

#ifndef RAWR_UNITY
    #pragma once
    #include "rawr/lib/detection_macros.hpp"
    #include "rawr/lib/simd/storage.hpp"
    #include "rawr/lib/dummy_return.hpp"
    #include "rawr/lib/detection.hpp"
    #include "rawr/cxx/intrin/macros.hpp"
    #include "rawr/data/aliases.hpp"
#endif

#if RAWR_COMPILER_MSVC
    // MSVC is quite picky about intrinsics, you have to *declare* them.
    // Its also very picky about the names of the types involved.
    // Somehow its not picky about whether or not they are namespaced.
    namespace rawr::arch::x64::msvc_detail
    {
        union __declspec(intrin_type) alignas(16) __m128 {
            u8  u8 [16];
            s8  s8 [16];
            u16 u16[8];
            s16 s16[8];
            u32 u32[4];
            s32 s32[4];
            f32 f32[4];
            u64 u64[2];
            s64 s64[2];
            f64 f64[2];
        };
        using __m128i = __m128;
        using __m128d = __m128;

        union __declspec(intrin_type) alignas(32) __m256 {
            u8  u8 [32];
            s8  s8 [32];
            u16 u16[16];
            s16 s16[16];
            u32 u32[8];
            s32 s32[8];
            f32 f32[8];
            u64 u64[4];
            s64 s64[4];
            f64 f64[4];
        };
        using __m256i = __m256;
        using __m256d = __m256;

        union __declspec(intrin_type) alignas(64) __m512 {
            u8  u8 [64];
            s8  s8 [64];
            u16 u16[32];
            s16 s16[32];
            u32 u32[16];
            s32 s32[16];
            f32 f32[16];
            u64 u64[8];
            s64 s64[8];
            f64 f64[8];
        };
        using __m512i = __m512;
        using __m512d = __m512;

        // These HAVE to be declared exactly like this, otherwise MSVC
        // complains. The text needs to be EXACTLY this, you can't alias
        // type names or have the type be a struct instead of an union or
        // anything like that either.
        extern "C" __m128 _mm_add_epi32(__m128 a, __m128 b);

        #pragma intrinsic(_mm_add_epi32)
    }
#endif

namespace rawr::arch::x64::sse
{
    // Software fallbacks.
    namespace sw
    {
        template <compilers C = this_compiler, archs A = this_arch>
        RAWR_ALWAYS_INLINE constexpr auto add_u32x4(simd::storage::u32x4 a, simd::storage::u32x4 b) -> simd::storage::u32x4
        {
            simd::storage::u32x4 ret{};
            for(auto i = 0; i < 4; ++i)
            { ret.lanes[i] = a.lanes[i] + b.lanes[i]; }
            return ret;
        }
    }

    template <compilers C = this_compiler, archs A = this_arch>
    RAWR_ALWAYS_INLINE constexpr auto add_u32x4(simd::storage::u32x4 a, simd::storage::u32x4 b) -> simd::storage::u32x4
    {
        #if RAWR_COMPILER_MSVC
            if(cxx::intrin::is_constexpr()) { return sw::add_u32x4(a, b); }

            auto v = msvc_detail::_mm_add_epi32(
                reinterpret_cast<msvc_detail::__m128&>(a),
                reinterpret_cast<msvc_detail::__m128&>(b)
            );
            return reinterpret_cast<simd::storage::u32x4&>(v);
        #elif RAWR_COMPILER_FAMILY_GNU
            return { a.lanes + b.lanes };
        #else
            static_assert("Unimplemented");
            return dummy_return{};
        #endif
    }
}

#pragma endregion rawr/arch/x64/simd.hpp
