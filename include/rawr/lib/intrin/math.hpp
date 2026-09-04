#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/intrin/math.hpp"
#endif

#include "rawr/lib/dist/todo.pp"

#ifdef RAWR_MODULE
    export module rawr.lib.intrin.math;
    import rawr.lib.intrin.base;
    import rawr.lib.integer.base;
    import rawr.lib.integer.raw;
    import rawr.lib.bits;
    import rawr.lib.detection;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once

    #include "rawr/lib/intrin/base.hpp"
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/bits.hpp"
    #include "rawr/lib/detection.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/compiler.pp"
#include "rawr/lib/attributes.pp"

#define RAWR_ASSERTION(...) // Dummy for now.

RAWR_EXPORT namespace rawr::inline lib::intrin::inline math::msvc
{
    using rulong = RAWR_MSVC_OR(unsigned long, ru32);

    RAWR_MSVC_INTRIN(1,             __popcnt16, (ru16) -> ru16);
    RAWR_MSVC_INTRIN(1,             __popcnt,   (ru32) -> ru32);
    RAWR_MSVC_INTRIN(RAWR_IS_64BIT, __popcnt64, (ru64) -> ru64);

    RAWR_MSVC_INTRIN(1,             _BitScanReverse,   (rulong*, rulong) -> ru8);
    RAWR_MSVC_INTRIN(RAWR_IS_64BIT, _BitScanReverse64, (rulong*, ru64)   -> ru8);
    RAWR_MSVC_INTRIN(1,             _BitScanForward,   (rulong*, rulong) -> ru8);
    RAWR_MSVC_INTRIN(RAWR_IS_64BIT, _BitScanForward64, (rulong*, ru64)   -> ru8);

    RAWR_MSVC_INTRIN(1, _byteswap_ushort, (ru16)   -> ru16);
    RAWR_MSVC_INTRIN(1, _byteswap_ulong,  (rulong) -> rulong);
    RAWR_MSVC_INTRIN(1, _byteswap_uint64, (ru64)   -> ru64);

    RAWR_MSVC_INTRIN(1, _rotl,   (ru32, rs32) -> ru32);
    RAWR_MSVC_INTRIN(1, _rotl64, (ru64, rs32) -> ru64);
    RAWR_MSVC_INTRIN(1, _rotr,   (ru32, rs32) -> ru32);
    RAWR_MSVC_INTRIN(1, _rotr64, (ru64, rs32) -> ru64);

    RAWR_MSVC_INTRIN(RAWR_ARCH_X64, _umul128, (ru64, ru64, ru64*)       -> ru64);
    RAWR_MSVC_INTRIN(RAWR_ARCH_X64, _mul128,  (rs64, rs64, rs64*)       -> rs64);
    RAWR_MSVC_INTRIN(RAWR_ARCH_X64, _udiv128, (ru64, ru64, ru64, ru64*) -> ru64);

    RAWR_MSVC_INTRIN(RAWR_ARCH_X86, _addcarry_u32,  (ru8, ru32, ru32, ru32*) -> ru8);
    RAWR_MSVC_INTRIN(RAWR_ARCH_X64, _addcarry_u64,  (ru8, ru64, ru64, ru64*) -> ru8);
    RAWR_MSVC_INTRIN(RAWR_ARCH_X86, _subborrow_u32, (ru8, ru32, ru32, ru32*) -> ru8);
    RAWR_MSVC_INTRIN(RAWR_ARCH_X64, _subborrow_u64, (ru8, ru64, ru64, ru64*) -> ru8);
}

// GNU builtin wrappers for popcount, clz, ctz, bswap.
RAWR_EXPORT namespace rawr::inline lib::intrin::inline math::gnu
{
    template <raint Raw>
    RAWR_ALWAYS_INLINE constexpr auto popcount(Raw val) noexcept -> Raw
    RAWR_GNU({
        auto const uval = static_cast<ruint_of<Raw>>(val);
             if constexpr (sizeof(Raw) <= sizeof(unsigned int))  { return static_cast<Raw>(__builtin_popcount (static_cast<unsigned int>      (uval))); }
        else if constexpr (sizeof(Raw) == sizeof(unsigned long)) { return static_cast<Raw>(__builtin_popcountl(static_cast<unsigned long>     (uval))); }
        else                                                     { return static_cast<Raw>(__builtin_popcountll(static_cast<unsigned long long>(uval))); }
    });

    template <raint Raw>
    RAWR_ALWAYS_INLINE constexpr auto leading_zeros(Raw val) noexcept -> Raw
    RAWR_GNU({
        using URaw = ruint_of<Raw>;
        constexpr auto bits  = static_cast<Raw>(bitsof<Raw>.val);
        auto const     uval  = static_cast<URaw>(val);
        if (uval == URaw{0}) { return bits; }
             if constexpr (sizeof(Raw) <= sizeof(unsigned int))  { return static_cast<Raw>(__builtin_clz  (static_cast<unsigned int>      (uval)) - (static_cast<unsigned int>(bitsof<unsigned int>.val) - static_cast<unsigned int>(bits))); }
        else if constexpr (sizeof(Raw) == sizeof(unsigned long)) { return static_cast<Raw>(__builtin_clzl (static_cast<unsigned long>     (uval))); }
        else                                                     { return static_cast<Raw>(__builtin_clzll(static_cast<unsigned long long>(uval))); }
    });

    template <raint Raw>
    RAWR_ALWAYS_INLINE constexpr auto trailing_zeros(Raw val) noexcept -> Raw
    RAWR_GNU({
        using URaw = ruint_of<Raw>;
        constexpr auto bits = static_cast<Raw>(bitsof<Raw>.val);
        URaw const     uval = static_cast<URaw>(val);
        if (uval == URaw{0}) { return bits; }
             if constexpr (sizeof(Raw) <= sizeof(unsigned int))  { return static_cast<Raw>(__builtin_ctz  (static_cast<unsigned int>      (uval))); }
        else if constexpr (sizeof(Raw) == sizeof(unsigned long)) { return static_cast<Raw>(__builtin_ctzl (static_cast<unsigned long>     (uval))); }
        else                                                     { return static_cast<Raw>(__builtin_ctzll(static_cast<unsigned long long>(uval))); }
    });

    template <ruint Raw>
    RAWR_ALWAYS_INLINE constexpr auto bswap(Raw val) noexcept -> Raw
    RAWR_GNU({
             if constexpr (sizeof(Raw) == 1) { return val; }
        else if constexpr (sizeof(Raw) == 2) { return static_cast<Raw>(__builtin_bswap16(static_cast<unsigned short>    (val))); }
        else if constexpr (sizeof(Raw) == 4) { return static_cast<Raw>(__builtin_bswap32(static_cast<unsigned int>      (val))); }
        else                                 { return static_cast<Raw>(__builtin_bswap64(static_cast<unsigned long long>(val))); }
    });
}

// popcount and family.
RAWR_EXPORT namespace rawr::inline lib::intrin::inline math
{
    namespace soft
    {
        template <raint Raw>
        [[nodiscard]] constexpr auto popcount(Raw val) noexcept -> Raw
        { Raw count = 0; while (val) { val &= static_cast<Raw>(val - Raw{1}); ++count; } return count; }

        template <raint Raw>
        [[nodiscard]] constexpr auto leading_zeros(Raw val) noexcept -> Raw
        {
            constexpr auto width = static_cast<Raw>(sizeof(Raw) * 8);
            if (val == Raw{0}) { return width; }
            Raw count = 0;
            Raw const msb = static_cast<Raw>(Raw{1} << (width - 1));
            while ((val & msb) == Raw{0}) { val = static_cast<Raw>(val << 1); ++count; }
            return count;
        }

        template <raint Raw>
        [[nodiscard]] constexpr auto trailing_zeros(Raw val) noexcept -> Raw
        {
            if (val == Raw{0}) { return static_cast<Raw>(bitsof<Raw>.val); }
            Raw count = 0;
            while ((val & Raw{1}) == Raw{0}) { val >>= 1; ++count; }
            return count;
        }

        template <raint Raw> [[nodiscard]] constexpr auto leading_ones(Raw val)  noexcept -> Raw { return leading_zeros(~val); }
        template <raint Raw> [[nodiscard]] constexpr auto trailing_ones(Raw val) noexcept -> Raw { return trailing_zeros(~val); }
    }

    template <raint Raw>
    [[nodiscard]] constexpr auto popcount(Raw val) noexcept -> Raw
    {
        if constexpr(this_compiler.is_family_gnu()) {
            return gnu::popcount(val);
        } else if constexpr(this_compiler.is_msvc()) {
            if (intrin::is_consteval()) return soft::popcount(val);
            auto const uval = static_cast<ruint_of<Raw>>(val);
                 if constexpr (sizeof(Raw) <= 2) { return static_cast<Raw>(msvc::__popcnt16(static_cast<ru16>(uval))); }
            else if constexpr (sizeof(Raw) == 4) { return static_cast<Raw>(msvc::__popcnt  (static_cast<ru32>(uval))); }
            else {
                // __popcnt64 is not available on 32-bit x86; split into two 32-bit calls.
                if constexpr(this_is_64bit) { return static_cast<Raw>(msvc::__popcnt64(static_cast<ru64>(uval))); }
                else {
                    return static_cast<Raw>(
                        msvc::__popcnt(static_cast<ru32>(uval)) +
                        msvc::__popcnt(static_cast<ru32>(static_cast<ru64>(uval) >> 32))
                    );
                }
            }
        } else {
            return soft::popcount(val);
        }
    }

    template <raint Raw>
    [[nodiscard]] constexpr auto leading_zeros(Raw val) noexcept -> Raw
    {
        if constexpr(this_compiler.is_family_gnu()) {
            return gnu::leading_zeros(val);
        } else if constexpr(this_compiler.is_msvc()) {
            using URaw = ruint_of<Raw>;
            constexpr auto bits = static_cast<Raw>(bitsof<Raw>.val);
            auto const     uval = static_cast<URaw>(val);
            if (intrin::is_consteval()) { return soft::leading_zeros(val); }
            if (uval == URaw{0}) { return bits; }
            msvc::rulong index;
            if constexpr (sizeof(Raw) <= 4) {
                msvc::_BitScanReverse(&index, static_cast<msvc::rulong>(uval));
                return static_cast<Raw>(31u - index - (32u - static_cast<unsigned int>(bits)));
            } else {
                // _BitScanReverse64 not available on 32-bit x86; split into two halves.
                if constexpr(this_is_64bit) {
                    msvc::_BitScanReverse64(&index, static_cast<ru64>(uval));
                    return static_cast<Raw>(63u - index);
                } else {
                    auto const hi = static_cast<msvc::rulong>(static_cast<ru64>(uval) >> 32);
                    auto const lo = static_cast<msvc::rulong>(uval);
                    if (hi != 0u) { msvc::_BitScanReverse(&index, hi); return static_cast<Raw>(31u - index); }
                    else          { msvc::_BitScanReverse(&index, lo); return static_cast<Raw>(63u - index); }
                }
            }
        } else {
            return soft::leading_zeros(val);
        }
    }

    template <raint Raw>
    [[nodiscard]] constexpr auto trailing_zeros(Raw val) noexcept -> Raw
    {
        if constexpr(this_compiler.is_family_gnu()) {
            return gnu::trailing_zeros(val);
        } else if constexpr(this_compiler.is_msvc()) {
            using URaw = ruint_of<Raw>;
            constexpr auto bits = static_cast<Raw>(bitsof<Raw>.val);
            URaw const     uval = static_cast<URaw>(val);
            if (intrin::is_consteval()) { return soft::trailing_zeros(val); }
            if (uval == URaw{0}) { return bits; }
            msvc::rulong index;
            if constexpr (sizeof(Raw) <= 4) {
                msvc::_BitScanForward(&index, static_cast<msvc::rulong>(uval));
            } else {
                // _BitScanForward64 not available on 32-bit x86; split into two halves.
                if constexpr(this_is_64bit) {
                    msvc::_BitScanForward64(&index, static_cast<ru64>(uval));
                } else {
                    auto const lo = static_cast<msvc::rulong>(uval);
                    if (lo != 0u) {
                        msvc::_BitScanForward(&index, lo);
                    } else {
                        msvc::_BitScanForward(&index, static_cast<msvc::rulong>(static_cast<ru64>(uval) >> 32));
                        index += 32u;
                    }
                }
            }
            return static_cast<Raw>(index);
        } else {
            return soft::trailing_zeros(val);
        }
    }

    template <raint Raw> [[nodiscard]] constexpr auto leading_ones(Raw val)  noexcept -> Raw { return leading_zeros(static_cast<Raw>(~val)); }
    template <raint Raw> [[nodiscard]] constexpr auto trailing_ones(Raw val) noexcept -> Raw { return trailing_zeros(static_cast<Raw>(~val)); }
}

// Byte-swap and rotation.
RAWR_EXPORT namespace rawr::inline lib::intrin::inline math
{
    namespace soft
    {
        template <ruint Raw>
        [[nodiscard]] constexpr auto bswap(Raw val) noexcept -> Raw
        {
                 if constexpr (sizeof(Raw) == 1) { return val; }
            else if constexpr (sizeof(Raw) == 2) {
                return static_cast<Raw>(
                    ((val & Raw{0x00FF}) <<  8) |
                    ((val & Raw{0xFF00}) >>  8)
                );
            }
            else if constexpr (sizeof(Raw) == 4) {
                return static_cast<Raw>(
                    ((val & 0x000000FFU) << 24) |
                    ((val & 0x0000FF00U) <<  8) |
                    ((val & 0x00FF0000U) >>  8) |
                    ((val & 0xFF000000U) >> 24)
                );
            }
            else {
                return static_cast<Raw>(
                    ((val & 0x00000000000000FFULL) << 56) |
                    ((val & 0x000000000000FF00ULL) << 40) |
                    ((val & 0x0000000000FF0000ULL) << 24) |
                    ((val & 0x00000000FF000000ULL) <<  8) |
                    ((val & 0x000000FF00000000ULL) >>  8) |
                    ((val & 0x0000FF0000000000ULL) >> 24) |
                    ((val & 0x00FF000000000000ULL) >> 40) |
                    ((val & 0xFF00000000000000ULL) >> 56)
                );
            }
        }

        // Normalize any int to [0, bits) — handles negative n (rotate right by k == rotate left by bits-k).
        template <ruint Raw>
        [[nodiscard]] constexpr auto rotl(Raw val, int n) noexcept -> Raw
        {
            constexpr int bits = static_cast<int>(bitsof<Raw>.val);
            n = ((n % bits) + bits) % bits;
            if (n == 0) { return val; }
            return static_cast<Raw>((val << static_cast<unsigned>(n)) | (val >> static_cast<unsigned>(bits - n)));
        }

        template <ruint Raw>
        [[nodiscard]] constexpr auto rotr(Raw val, int n) noexcept -> Raw
        { return soft::rotl(val, -n); }
    }

    // Byte-swap. Restricted to unsigned — signed bswap has no meaningful interpretation.
    template <ruint Raw>
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto bswap(Raw val) noexcept -> Raw
    {
        if constexpr(this_compiler.is_family_gnu()) {
            return gnu::bswap(val);
        } else if constexpr(this_compiler.is_msvc()) {
            if (intrin::is_consteval()) { return soft::bswap(val); }
                 if constexpr (sizeof(Raw) == 1) { return val; }
            else if constexpr (sizeof(Raw) == 2) { return static_cast<Raw>(msvc::_byteswap_ushort(static_cast<ru16>         (val))); }
            else if constexpr (sizeof(Raw) == 4) { return static_cast<Raw>(msvc::_byteswap_ulong (static_cast<msvc::rulong> (val))); }
            else                                 { return static_cast<Raw>(msvc::_byteswap_uint64(static_cast<ru64>         (val))); }
        } else {
            return soft::bswap(val);
        }
    }

    RAWR_TODO("fix rotation on gcc to the builtin when it exists -> check compiler version.")

    // Rotation. Negative n rotates in the opposite direction (matching std::rotl/rotr semantics).
    // GCC ≥ 12 and Clang ≥ 8 have __builtin_rotateleft*, but GCC 11 (minimum supported) does not.
    // The idiom in soft:: is recognised by all supported compilers and lowers to a single ROL/ROR on x86.
    template <ruint Raw>
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto rotl(Raw val, int n) noexcept -> Raw
    {
        if constexpr(this_compiler.is_family_gnu()) {
            return soft::rotl(val, n);
        } else if constexpr(this_compiler.is_msvc()) {
            if (intrin::is_consteval()) { return soft::rotl(val, n); }
            constexpr int bits  = static_cast<int>(bitsof<Raw>.val);
            int const     norm  = ((n % bits) + bits) % bits;
            if (norm == 0) { return val; }
                 if constexpr (sizeof(Raw) == 4) { return static_cast<Raw>(msvc::_rotl  (static_cast<ru32>(val), norm)); }
            else if constexpr (sizeof(Raw) == 8) { return static_cast<Raw>(msvc::_rotl64(static_cast<ru64>(val), norm)); }
            else { return static_cast<Raw>((val << static_cast<unsigned>(norm)) | (val >> static_cast<unsigned>(bits - norm))); }
        } else {
            return soft::rotl(val, n);
        }
    }

    template <ruint Raw>
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto rotr(Raw val, int n) noexcept -> Raw
    {
        if constexpr(this_compiler.is_family_gnu()) {
            return soft::rotr(val, n);
        } else if constexpr(this_compiler.is_msvc()) {
            if (intrin::is_consteval()) { return soft::rotr(val, n); }
            constexpr int bits  = static_cast<int>(bitsof<Raw>.val);
            int const     norm  = ((n % bits) + bits) % bits;
            if (norm == 0) { return val; }
                 if constexpr (sizeof(Raw) == 4) { return static_cast<Raw>(msvc::_rotr  (static_cast<ru32>(val), norm)); }
            else if constexpr (sizeof(Raw) == 8) { return static_cast<Raw>(msvc::_rotr64(static_cast<ru64>(val), norm)); }
            else { return static_cast<Raw>((val << static_cast<unsigned>(bits - norm)) | (val >> static_cast<unsigned>(norm))); }
        } else {
            return soft::rotr(val, n);
        }
    }
}

// umul64 and udiv128_64 — types and soft:: only.
// gnu:: wrappers follow, then public functions, to respect the ordering constraint
// (gnu:: return types must be defined before the wrapper declarations).
RAWR_EXPORT namespace rawr::inline lib::intrin::inline math
{
    struct ru64_pair {
        ru64 hi = 0;
        ru64 lo = 0;
    };
    namespace soft
    {
        [[nodiscard]] constexpr auto umul64(ru64 lhs, ru64 rhs) noexcept -> ru64_pair
        {
            constexpr ru64 LOW_MASK  = 0xffffffffULL;
            constexpr int  HALF_BITS = 32;

            ru64 lhs_lo = lhs & LOW_MASK;
            ru64 lhs_hi = lhs >> HALF_BITS;
            ru64 rhs_lo = rhs & LOW_MASK;
            ru64 rhs_hi = rhs >> HALF_BITS;

            ru64 prod_lo_lo = lhs_lo * rhs_lo;
            ru64 prod_lo_hi = lhs_lo * rhs_hi;
            ru64 prod_hi_lo = lhs_hi * rhs_lo;
            ru64 prod_hi_hi = lhs_hi * rhs_hi;

            ru64 middle_carry =
                (prod_lo_lo >> HALF_BITS) +
                (prod_lo_hi & LOW_MASK) +
                (prod_hi_lo & LOW_MASK);

            return {
                .hi = prod_hi_hi
                    + (prod_lo_hi >> HALF_BITS)
                    + (prod_hi_lo >> HALF_BITS)
                    + (middle_carry >> HALF_BITS),
                .lo = (middle_carry << HALF_BITS) | (prod_lo_lo & LOW_MASK)
            };
        }
    }

    struct u128_div_result {
        ru64 quot = 0;
        ru64 rem  = 0;
    };
    namespace soft {
        constexpr auto udiv128_64(ru64_pair ru64s, ru64 divisor) noexcept -> u128_div_result
        {
            RAWR_ASSERTION(divisor_is_zero_tag, divisor != 0);
            RAWR_ASSERTION(div128_overflow_tag, ru64s.hi < divisor);

            if (ru64s.hi == 0) { return { .quot = ru64s.lo / divisor, .rem = ru64s.lo % divisor }; }

            ru64 quot = 0;
            ru64 rem  = ru64s.hi;

            for (int i = bitsof<ru64>.val - 1; i >= 0; --i) {
                ru64 const bit      = (ru64s.lo >> i) & 1ULL;
                bool const overflow = (rem >> 63) != 0;
                rem = (rem << 1) | bit;
                if (overflow || rem >= divisor) {
                    rem -= divisor;
                    quot |= (1ULL << i);
                }
            }
            return { .quot = quot, .rem = rem };
        }
    }
}

RAWR_EXPORT namespace rawr::inline lib::intrin::inline math::gnu
{
    RAWR_ALWAYS_INLINE constexpr auto umul64(ru64 lhs, ru64 rhs) noexcept -> ru64_pair
    RAWR_GNU_COND(RAWR_HAS_INT128, {
        ru128 ret = static_cast<ru128>(lhs) * rhs;
        return {
            .hi = static_cast<ru64>(ret >> bitsof<ru64>.val),
            .lo = static_cast<ru64>(ret)
        };
    });

    RAWR_ALWAYS_INLINE constexpr auto udiv128_64(ru64_pair ru64s, ru64 divisor) noexcept -> u128_div_result
    RAWR_GNU_COND(RAWR_HAS_INT128, {
        ru128 num = (static_cast<ru128>(ru64s.hi) << bitsof<ru64>.val) | ru64s.lo;
        return {
            .quot = static_cast<ru64>(num / divisor),
            .rem  = static_cast<ru64>(num % divisor)
        };
    });
}

RAWR_EXPORT namespace rawr::inline lib::intrin::inline math
{
    // Full unsigned 64 x 64 -> 128 product.
    constexpr auto umul64(ru64 lhs, ru64 rhs) noexcept -> ru64_pair
    {
        if constexpr(this_compiler.is_family_gnu() && this_has_int128) {
            return gnu::umul64(lhs, rhs);
        } else if constexpr(this_compiler.is_msvc() && this_arch.is_x64()) {
            if (intrin::is_consteval()) { return soft::umul64(lhs, rhs); }
            ru64_pair ret;
            ret.lo = msvc::_umul128(lhs, rhs, &ret.hi);
            return ret;
        } else {
            return soft::umul64(lhs, rhs);
        }
    }

    constexpr auto udiv128_64(ru64_pair ru64s, ru64 divisor) noexcept -> u128_div_result
    {
        RAWR_ASSERTION(divisor_is_zero, divisor != 0);
        RAWR_ASSERTION(div128_overflow, ru64s.hi < divisor);

        if constexpr(this_compiler.is_family_gnu() && this_has_int128) {
            return gnu::udiv128_64(ru64s, divisor);
        } else if constexpr(this_compiler.is_msvc() && this_arch.is_x64()) {
            if (intrin::is_consteval()) { return soft::udiv128_64(ru64s, divisor); }
            ru64 rem  = 0;
            ru64 quot = msvc::_udiv128(ru64s.hi, ru64s.lo, divisor, &rem);
            return { quot, rem };
        } else {
            return soft::udiv128_64(ru64s, divisor);
        }
    }
}

// Overflow arithmetic — ov_result and soft:: only.
// gnu:: wrappers follow, then public functions.
RAWR_EXPORT namespace rawr::inline lib::intrin::inline math
{
    // Unconstrained deliberately — see note above. It's a passive holder;
    // constraining T against `aint` here is what caused the CRTP hazard.
    template <typename T> struct ov_result { T val{}; bool overflowed = false; };

    namespace soft
    {
        template <rsint Raw>
        [[nodiscard]] constexpr auto did_add_overflow(Raw lhs, Raw rhs, Raw result) noexcept -> bool
        { return (lhs > 0 && rhs > 0 && result < 0) || (lhs < 0 && rhs < 0 && result >= 0); }
        template <rsint Raw>
        [[nodiscard]] constexpr auto did_sub_underflow(Raw lhs, Raw rhs, Raw result) noexcept -> bool
        { return (lhs >= 0 && rhs < 0 && result < 0) || (lhs < 0 && rhs >= 0 && result >= 0); }

        template <raint Raw>
        [[nodiscard]] constexpr auto ov_add(Raw lhs_, Raw rhs_) noexcept -> ov_result<Raw>
        {
            auto const lhs    = static_cast<ruint_of<Raw>>(lhs_);
            auto const rhs    = static_cast<ruint_of<Raw>>(rhs_);
            Raw const  result = static_cast<Raw>( lhs + rhs );
            if constexpr (uint<Raw>) { return { result, static_cast<ruint_of<Raw>>(lhs + rhs) < lhs }; }
            else                     { return { result, did_add_overflow(lhs_, rhs_, result) }; }
        }

        template <raint Raw>
        [[nodiscard]] constexpr auto ov_sub(Raw lhs_, Raw rhs_) noexcept -> ov_result<Raw>
        {
            auto const lhs    = static_cast<ruint_of<Raw>>(lhs_);
            auto const rhs    = static_cast<ruint_of<Raw>>(rhs_);
            Raw const  result = static_cast<Raw>( lhs - rhs );
            if constexpr (uint<Raw>) { return { result, lhs < rhs }; }
            else                     { return { result, did_sub_underflow(lhs_, rhs_, result) }; }
        }

        template <ruint Raw> requires (sizeof(Raw) <= 4)
        [[nodiscard]] constexpr auto ov_mul(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
        {
            ru64 const wider = static_cast<ru64>(lhs) * static_cast<ru64>(rhs);
            return { static_cast<Raw>(wider), wider > static_cast<ru64>(aint_max<Raw>) };
        }

        template <rsint Raw> requires (sizeof(Raw) <= 4)
        [[nodiscard]] constexpr auto ov_mul(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
        {
            rs64 const wider = static_cast<rs64>(lhs) * static_cast<rs64>(rhs);
            return {
                static_cast<Raw>(wider),
                wider > static_cast<rs64>(aint_max<Raw>) || wider < static_cast<rs64>(aint_min<Raw>)
            };
        }

        template <ruint64 Raw>
        [[nodiscard]] constexpr auto ov_mul(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
        {
            auto const ret = intrin::umul64(lhs, rhs);
            return { static_cast<Raw>(ret.lo), ret.hi != 0 };
        }

        template <rsint64 Raw>
        [[nodiscard]] constexpr auto ov_mul(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
        {
            if (lhs == 0 || rhs == 0) { return { Raw{0}, false }; }

            auto magnitude = [](Raw val) -> ru64 { return val < 0 ? (ru64{0} - static_cast<ru64>(val)) : static_cast<ru64>(val); };
            auto const ret = umul64(magnitude(lhs), magnitude(rhs));

            bool const same_sign = (lhs < 0) == (rhs < 0);
            if (same_sign) {
                bool const overflowed = ret.hi != 0 || ret.lo > static_cast<ru64>(aint_max<Raw>);
                return { static_cast<Raw>(ret.lo), overflowed };
            }

            ru64 const min_mag   = static_cast<ru64>(aint_max<Raw>) + 1ULL;
            bool const overflowed = ret.hi != 0 || ret.lo > min_mag;
            return { static_cast<Raw>(ru64{0} - ret.lo), overflowed };
        }
    }
}

RAWR_EXPORT namespace rawr::inline lib::intrin::inline math::gnu
{
    template <raint Raw>
    RAWR_ALWAYS_INLINE constexpr auto ov_add(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
    RAWR_GNU({
        Raw result;
        bool const overflowed = __builtin_add_overflow(lhs, rhs, &result);
        return { result, overflowed };
    });

    template <raint Raw>
    RAWR_ALWAYS_INLINE constexpr auto ov_sub(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
    RAWR_GNU({
        Raw result;
        bool const overflowed = __builtin_sub_overflow(lhs, rhs, &result);
        return { result, overflowed };
    });

    template <raint Raw>
    RAWR_ALWAYS_INLINE constexpr auto ov_mul(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
    RAWR_GNU({
        Raw result;
        bool const overflowed = __builtin_mul_overflow(lhs, rhs, &result);
        return { result, overflowed };
    });
}

RAWR_EXPORT namespace rawr::inline lib::intrin::inline math
{
    template <raint Raw>
    [[nodiscard]] constexpr auto ov_add(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
    {
        if constexpr(this_compiler.is_family_gnu()) {
            return gnu::ov_add(lhs, rhs);
        } else if constexpr(this_compiler.is_msvc()) {
            if (intrin::is_consteval()) return soft::ov_add(lhs, rhs);
            if constexpr (sizeof(Raw) == 4) {
                // _addcarry_u32 is x86-only (32-bit); on x64 the soft path is taken.
                if constexpr(this_arch.is_x86()) {
                    ru32 result_;
                    ru8 const carry = msvc::_addcarry_u32(0, static_cast<ru32>(lhs), static_cast<ru32>(rhs), &result_);
                    Raw const result = static_cast<Raw>(result_);
                    if constexpr (uint<Raw>) return { result, carry != 0 };
                    else                    return { result, soft::did_add_overflow(lhs, rhs, result) };
                } else { return soft::ov_add(lhs, rhs); }
            } else if constexpr (sizeof(Raw) == 8) {
                // _addcarry_u64 is x64-only.
                if constexpr(this_arch.is_x64()) {
                    ru64 result_;
                    ru8 const carry = msvc::_addcarry_u64(0, static_cast<ru64>(lhs), static_cast<ru64>(rhs), &result_);
                    Raw const result = static_cast<Raw>(result_);
                    if constexpr (uint<Raw>) return { result, carry != 0 };
                    else                    return { result, soft::did_add_overflow(lhs, rhs, result) };
                } else { return soft::ov_add(lhs, rhs); }
            } else { return soft::ov_add(lhs, rhs); }
        } else {
            return soft::ov_add(lhs, rhs);
        }
    }

    template <raint Raw>
    [[nodiscard]] constexpr auto ov_sub(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
    {
        if constexpr(this_compiler.is_family_gnu()) {
            return gnu::ov_sub(lhs, rhs);
        } else if constexpr(this_compiler.is_msvc()) {
            if (intrin::is_consteval()) return soft::ov_sub(lhs, rhs);
            if constexpr (sizeof(Raw) == 4) {
                // _subborrow_u32 is x86-only (32-bit); on x64 the soft path is taken.
                if constexpr(this_arch.is_x86()) {
                    ru32 result_;
                    ru8 const carry = msvc::_subborrow_u32(0, static_cast<ru32>(lhs), static_cast<ru32>(rhs), &result_);
                    Raw const result = static_cast<Raw>(result_);
                    if constexpr (uint<Raw>) return { result, carry != 0 };
                    else                    return { result, soft::did_sub_underflow(lhs, rhs, result) };
                } else { return soft::ov_sub(lhs, rhs); }
            } else if constexpr (sizeof(Raw) == 8) {
                // _subborrow_u64 is x64-only.
                if constexpr(this_arch.is_x64()) {
                    ru64 result_;
                    ru8 const carry = msvc::_subborrow_u64(0, static_cast<ru64>(lhs), static_cast<ru64>(rhs), &result_);
                    Raw const result = static_cast<Raw>(result_);
                    if constexpr (uint<Raw>) return { result, carry != 0 };
                    else                    return { result, soft::did_sub_underflow(lhs, rhs, result) };
                } else { return soft::ov_sub(lhs, rhs); }
            } else { return soft::ov_sub(lhs, rhs); }
        } else {
            return soft::ov_sub(lhs, rhs);
        }
    }

    template <raint Raw>
    [[nodiscard]] constexpr auto ov_mul(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
    {
        if constexpr(this_compiler.is_family_gnu()) {
            return gnu::ov_mul(lhs, rhs);
        } else if constexpr(this_compiler.is_msvc()) {
            if constexpr (sizeof(Raw) <= 4) { return soft::ov_mul(lhs, rhs); }
            else {
                // _umul128/_mul128 are x64-only; arm64 and x86 MSVC fall back to soft.
                if constexpr(this_arch.is_x64()) {
                    if (intrin::is_consteval()) return soft::ov_mul(lhs, rhs);
                    if constexpr (uint<Raw>) {
                        ru64 high;
                        ru64 low = msvc::_umul128(static_cast<ru64>(lhs), static_cast<ru64>(rhs), &high);
                        return { static_cast<Raw>(low), high != 0 };
                    } else {
                        rs64 high;
                        rs64 low = msvc::_mul128(static_cast<rs64>(lhs), static_cast<rs64>(rhs), &high);
                        return { static_cast<Raw>(low), high != (low >> 63) };
                    }
                } else { return soft::ov_mul(lhs, rhs); }
            }
        } else {
            return soft::ov_mul(lhs, rhs);
        }
    }
}
