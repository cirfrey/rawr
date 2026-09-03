//// rawr/arch/x64/simd.hpp.

#ifdef RAWR_MODULE
    export module rawr.arch.x64.simd;
    import rawr.lib.simd.storage;
    import rawr.lib.dummy_return;
    import rawr.lib.detection;
    import rawr.lib.integer.raw;
    import rawr.lib.intrin;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/simd/storage.hpp"
    #include "rawr/lib/dummy_return.hpp"
    #include "rawr/lib/detection.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/intrin.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/attributes.pp"
#include "rawr/lib/detection.pp"
#include "rawr/lib/compiler.pp"

// MSVC is quite picky about intrinsics, you have to *declare* them.
// Its also very picky about the names of the types involved.
// Somehow its not picky about whether or not they are namespaced.
namespace rawr::arch::x64::msvc
{
    union RAWR_DECLSPEC(intrin_type) alignas(16) __m128 {
        rawr::ru8  ru8 [16];
        rawr::rs8  rs8 [16];
        rawr::ru16 ru16[8];
        rawr::rs16 rs16[8];
        rawr::ru32 ru32[4];
        rawr::rs32 rs32[4];
        rawr::rf32 rf32[4];
        rawr::ru64 ru64[2];
        rawr::rs64 rs64[2];
        rawr::rf64 rf64[2];
    };
    using __m128i = __m128;
    using __m128d = __m128;

    union RAWR_DECLSPEC(intrin_type) alignas(32) __m256 {
        rawr::ru8  ru8 [32];
        rawr::rs8  rs8 [32];
        rawr::ru16 ru16[16];
        rawr::rs16 rs16[16];
        rawr::ru32 ru32[8];
        rawr::rs32 rs32[8];
        rawr::rf32 rf32[8];
        rawr::ru64 ru64[4];
        rawr::rs64 rs64[4];
        rawr::rf64 rf64[4];
    };
    using __m256i = __m256;
    using __m256d = __m256;

    union RAWR_DECLSPEC(intrin_type) alignas(64) __m512 {
        rawr::ru8  ru8 [64];
        rawr::rs8  rs8 [64];
        rawr::ru16 ru16[32];
        rawr::rs16 rs16[32];
        rawr::ru32 ru32[16];
        rawr::rs32 rs32[16];
        rawr::rf32 rf32[16];
        rawr::ru64 ru64[8];
        rawr::rs64 rs64[8];
        rawr::rf64 rf64[8];
    };
    using __m512i = __m512;
    using __m512d = __m512;

    // These HAVE to be declared exactly like this, otherwise MSVC
    // complains. The text needs to be EXACTLY this, you can't alias
    // type names or have the type be a struct instead of an union or
    // anything like that either.
    RAWR_MSVC_INTRIN(RAWR_ARCH_X64, _mm_add_epi32, (__m128i, __m128i) -> __m128i);
}

namespace rawr::arch::x64::sse
{
    namespace soft
    {
        template <compilers C = this_compiler, archs A = this_arch>
        RAWR_ALWAYS_INLINE constexpr auto add_u32x4(simd::storage::ru32x4 lhs, simd::storage::ru32x4 rhs) -> simd::storage::ru32x4
        {
            simd::storage::ru32x4 ret{};
            for(auto i = 0; i < 4; ++i)
            { ret.lanes[i] = lhs.lanes[i] + rhs.lanes[i]; }
            return ret;
        }
    }

    template <compilers C = this_compiler, archs A = this_arch>
    RAWR_ALWAYS_INLINE constexpr auto add_u32x4(simd::storage::ru32x4 lhs, simd::storage::ru32x4 rhs) -> simd::storage::ru32x4
    {
        if constexpr(!this_arch.is_x64()) {
            static_assert("Unimplemented");
            return dummy_return{};
        } else if constexpr(this_compiler.is_family_gnu()) {
            return { lhs.lanes + rhs.lanes };
        } else if constexpr(this_compiler.is_msvc()) {
            if(intrin::is_consteval()) { return soft::add_u32x4(lhs, rhs); }
            auto val = msvc::_mm_add_epi32(
                reinterpret_cast<msvc::__m128&>(lhs),
                reinterpret_cast<msvc::__m128&>(rhs)
            );
            return reinterpret_cast<simd::storage::ru32x4&>(val);
        }
    }
}
