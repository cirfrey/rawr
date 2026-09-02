//// rawr/lib/intrin/math.hpp.

#ifdef RAWR_MODULE
    export module rawr.lib.intrin.math;
    import rawr.lib.intrin.base;
    import rawr.lib.integer.base;
    import rawr.lib.integer.raw;
    import rawr.lib.bits;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once

    #include "rawr/lib/intrin/base.hpp"
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/bits.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"
#include "rawr/lib/attributes.pp"

#define RAWR_ASSERTION(...) // Dummy for now.

// TODO: this might also need an arch guard.
#if RAWR_COMPILER_MSVC
    namespace rawr::inline lib::intrin::inline math::msvc
    {
        extern "C" unsigned short   __popcnt16(unsigned short);
        extern "C" unsigned int     __popcnt(unsigned int);
        extern "C" unsigned __int64 __popcnt64(unsigned __int64);
        #pragma intrinsic(__popcnt16, __popcnt, __popcnt64)

        extern "C" unsigned char _BitScanReverse(unsigned long*, unsigned long);
        extern "C" unsigned char _BitScanReverse64(unsigned long*, unsigned __int64);
        extern "C" unsigned char _BitScanForward(unsigned long*, unsigned long);
        extern "C" unsigned char _BitScanForward64(unsigned long*, unsigned __int64);
        #pragma intrinsic(_BitScanReverse, _BitScanReverse64, _BitScanForward, _BitScanForward64)

        extern "C" unsigned short   _byteswap_ushort(unsigned short);
        extern "C" unsigned long    _byteswap_ulong(unsigned long);
        extern "C" unsigned __int64 _byteswap_uint64(unsigned __int64);
        #pragma intrinsic(_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)

        extern "C" unsigned int     _rotl  (unsigned int,      int);
        extern "C" unsigned __int64 _rotl64(unsigned __int64,  int);
        extern "C" unsigned int     _rotr  (unsigned int,      int);
        extern "C" unsigned __int64 _rotr64(unsigned __int64,  int);
        #pragma intrinsic(_rotl,_rotl64, _rotr, _rotr64)

        extern "C" unsigned __int64 _umul128(unsigned __int64, unsigned __int64, unsigned __int64*);
        extern "C"          __int64 _mul128(__int64, __int64, __int64*);
        extern "C" unsigned __int64 _udiv128(unsigned __int64, unsigned __int64, unsigned __int64, unsigned __int64*);
        #pragma intrinsic(_umul128, _mul128, _udiv128)

        extern "C" unsigned char _addcarry_u32(unsigned char, unsigned int, unsigned int, unsigned int*);
        extern "C" unsigned char _addcarry_u64(unsigned char, unsigned __int64, unsigned __int64, unsigned __int64*);
        extern "C" unsigned char _subborrow_u32(unsigned char, unsigned int, unsigned int, unsigned int*);
        extern "C" unsigned char _subborrow_u64(unsigned char, unsigned __int64, unsigned __int64, unsigned __int64*);
        #pragma intrinsic(_addcarry_u32, _addcarry_u64, _subborrow_u32, _subborrow_u64)
    }
#endif

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
        auto const uval = static_cast< ruint_of<Raw> >(val);

        #if RAWR_COMPILER_FAMILY_GNU
                 if constexpr (sizeof(Raw) <= sizeof(unsigned int))  { return static_cast<Raw>(__builtin_popcount(static_cast<unsigned int>(uval))); }
            else if constexpr (sizeof(Raw) == sizeof(unsigned long)) { return static_cast<Raw>(__builtin_popcountl(uval)); }
            else                                                     { return static_cast<Raw>(__builtin_popcountll(uval)); }
        #elif RAWR_COMPILER_MSVC
            if (intrin::is_consteval()) return soft::popcount(val);
                 if constexpr (sizeof(Raw) <= 2) { return static_cast<Raw>(msvc::__popcnt16(static_cast<unsigned short>(uval))); }
            else if constexpr (sizeof(Raw) == 4) { return static_cast<Raw>(msvc::__popcnt(uval)); }
            else                                 { return static_cast<Raw>(msvc::__popcnt64(uval)); }
        #else
            return soft::popcount(val);
        #endif
    }

    template <raint Raw>
    [[nodiscard]] constexpr auto leading_zeros(Raw val) noexcept -> Raw
    {
        using URaw = ruint_of<Raw>;
        constexpr auto bits = static_cast<Raw>(bitsof<Raw>.val);
        auto const uval = static_cast<URaw>(val);

        #if RAWR_COMPILER_FAMILY_GNU
            if (uval == URaw{0}) { return bits; }

                 if constexpr (sizeof(Raw) <= sizeof(unsigned int))  { return static_cast<Raw>(__builtin_clz(static_cast<unsigned int>(uval)) - (static_cast<unsigned int>(bitsof<unsigned int>.val) - static_cast<unsigned int>(bits))); }
            else if constexpr (sizeof(Raw) == sizeof(unsigned long)) { return static_cast<Raw>(__builtin_clzl(uval)); }
            else                                                     { return static_cast<Raw>(__builtin_clzll(uval)); }
        #elif RAWR_COMPILER_MSVC
            if (intrin::is_consteval()) { return soft::leading_zeros(val); }
            if (uval == URaw{0}) { return bits; }
            unsigned long index;
            if constexpr (sizeof(Raw) <= 4) {
                msvc::_BitScanReverse(&index, static_cast<unsigned long>(uval));
                return static_cast<Raw>(31u - index - (32u - static_cast<unsigned int>(bits)));
            }
            else {
                msvc::_BitScanReverse64(&index, uval);
                return static_cast<Raw>(63u - index);
            }
        #else
            return soft::leading_zeros(val);
        #endif
    }

    template <raint Raw>
    [[nodiscard]] constexpr auto trailing_zeros(Raw val) noexcept -> Raw
    {
        using URaw = ruint_of<Raw>;
        constexpr auto bits = static_cast<Raw>(bitsof<Raw>.val);
        URaw const uval = static_cast<URaw>(val);

        #if RAWR_COMPILER_FAMILY_GNU
            if (uval == URaw{0}) { return bits; }

                 if constexpr (sizeof(Raw) <= sizeof(unsigned int))  { return static_cast<Raw>(__builtin_ctz(static_cast<unsigned int>(uval))); }
            else if constexpr (sizeof(Raw) == sizeof(unsigned long)) { return static_cast<Raw>(__builtin_ctzl(uval)); }
            else                                                     { return static_cast<Raw>(__builtin_ctzll(uval)); }
        #elif RAWR_COMPILER_MSVC
            if (intrin::is_consteval()) { return soft::trailing_zeros(val); }

            if (uval == URaw{0}) { return bits; }

            unsigned long index;
            if constexpr (sizeof(Raw) <= 4) { msvc::_BitScanForward(&index, static_cast<unsigned long>(uval)); }
            else                            { msvc::_BitScanForward64(&index, uval); }

            return static_cast<Raw>(index);
        #else
            return soft::trailing_zeros(val);
        #endif
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
        #if RAWR_COMPILER_FAMILY_GNU
                 if constexpr (sizeof(Raw) == 1) { return val; }
            else if constexpr (sizeof(Raw) == 2) { return static_cast<Raw>(__builtin_bswap16(static_cast<unsigned short>(val)));         }
            else if constexpr (sizeof(Raw) == 4) { return static_cast<Raw>(__builtin_bswap32(static_cast<unsigned int>(val)));           }
            else                                 { return static_cast<Raw>(__builtin_bswap64(static_cast<unsigned long long>(val)));     }
        #elif RAWR_COMPILER_MSVC
            if (intrin::is_consteval()) { return soft::bswap(val); }
                 if constexpr (sizeof(Raw) == 1) { return val; }
            else if constexpr (sizeof(Raw) == 2) { return static_cast<Raw>(msvc::_byteswap_ushort(static_cast<unsigned short>(val)));    }
            else if constexpr (sizeof(Raw) == 4) { return static_cast<Raw>(msvc::_byteswap_ulong(static_cast<unsigned long>(val)));      }
            else                                 { return static_cast<Raw>(msvc::_byteswap_uint64(static_cast<unsigned __int64>(val)));  }
        #else
            return soft::bswap(val);
        #endif
    }

    // TODO: fix rotation on gcc to the builtin when it exists -> check compiler version.

    // Rotation. Negative n rotates in the opposite direction (matching std::rotl/rotr semantics).
    // GCC ≥ 12 and Clang ≥ 8 have __builtin_rotateleft*, but GCC 11 (minimum supported) does not.
    // The idiom in soft:: is recognised by all supported compilers and lowers to a single ROL/ROR on x86.
    template <ruint Raw>
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto rotl(Raw val, int n) noexcept -> Raw
    {
        #if RAWR_COMPILER_FAMILY_GNU
            return soft::rotl(val, n);
        #elif RAWR_COMPILER_MSVC
            if (intrin::is_consteval()) { return soft::rotl(val, n); }
            constexpr int bits = static_cast<int>(bitsof<Raw>.val);
            int const norm = ((n % bits) + bits) % bits;
            if (norm == 0) { return val; }
                 if constexpr (sizeof(Raw) == 4) { return static_cast<Raw>(msvc::_rotl  (static_cast<unsigned int>      (val), norm)); }
            else if constexpr (sizeof(Raw) == 8) { return static_cast<Raw>(msvc::_rotl64(static_cast<unsigned __int64>  (val), norm)); }
            else { return static_cast<Raw>((val << static_cast<unsigned>(norm)) | (val >> static_cast<unsigned>(bits - norm))); }
        #else
            return soft::rotl(val, n);
        #endif
    }

    template <ruint Raw>
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto rotr(Raw val, int n) noexcept -> Raw
    {
        #if RAWR_COMPILER_FAMILY_GNU
            return soft::rotr(val, n);
        #elif RAWR_COMPILER_MSVC
            if (intrin::is_consteval()) { return soft::rotr(val, n); }
            constexpr int bits = static_cast<int>(bitsof<Raw>.val);
            int const norm = ((n % bits) + bits) % bits;
            if (norm == 0) { return val; }
                 if constexpr (sizeof(Raw) == 4) { return static_cast<Raw>(msvc::_rotr  (static_cast<unsigned int>      (val), norm)); }
            else if constexpr (sizeof(Raw) == 8) { return static_cast<Raw>(msvc::_rotr64(static_cast<unsigned __int64>  (val), norm)); }
            else { return static_cast<Raw>((val << static_cast<unsigned>(bits - norm)) | (val >> static_cast<unsigned>(norm))); }
        #else
            return soft::rotr(val, n);
        #endif
    }
}

// umul64 and udiv128_64.
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

            // Split operands into low and high 32‑bit halves
            ru64 lhs_lo = lhs & LOW_MASK;
            ru64 lhs_hi = lhs >> HALF_BITS;
            ru64 rhs_lo = rhs & LOW_MASK;
            ru64 rhs_hi = rhs >> HALF_BITS;

            // Four 32‑bit multiplications (each yields a 64‑bit product)
            ru64 prod_lo_lo = lhs_lo * rhs_lo;
            ru64 prod_lo_hi = lhs_lo * rhs_hi;
            ru64 prod_hi_lo = lhs_hi * rhs_lo;
            ru64 prod_hi_hi = lhs_hi * rhs_hi;

            // Middle column: sum of the low parts of cross‑products and the carry
            // from the low‑low product's high half.
            ru64 middle_carry =
                (prod_lo_lo >> HALF_BITS) +
                (prod_lo_hi & LOW_MASK) +
                (prod_hi_lo & LOW_MASK);

            // Assemble the final 128‑bit result
            return {
                .hi = prod_hi_hi
                    + (prod_lo_hi >> HALF_BITS)
                    + (prod_hi_lo >> HALF_BITS)
                    + (middle_carry >> HALF_BITS),
                .lo = (middle_carry << HALF_BITS) | (prod_lo_lo & LOW_MASK)
            };
        }
    }
    // Full unsigned 64 x 64 -> 128 product
    constexpr auto umul64(ru64 lhs, ru64 rhs) noexcept -> ru64_pair
    {
        #if RAWR_COMPILER_MSVC
            if (intrin::is_consteval()) { return soft::umul64(lhs, rhs); }
            ru64_pair ret;
            ret.lo = msvc::_umul128(lhs, rhs, &ret.hi);
            return ret;
        #else
            unsigned __int128 ret = static_cast<unsigned __int128>(lhs) * rhs;
            return {
                .hi = static_cast<ru64>(ret >> bitsof<ru64>.val),
                .lo = static_cast<ru64>(ret)
            };
        #endif
    }

    struct u128_div_result {
        ru64 quot = 0;
        ru64 rem  = 0;
    };
    namespace soft{
        constexpr auto udiv128_64(ru64_pair ru64s, ru64 divisor) noexcept -> u128_div_result
        {
            RAWR_ASSERTION(divisor_is_zero_tag,   divisor != 0);
            RAWR_ASSERTION(div128_overflow_tag,   ru64s.hi < divisor);

            if (ru64s.hi == 0) { return { .quot = ru64s.lo / divisor, .rem = ru64s.lo % divisor }; }

            // Under precondition hi < divisor, no subtract fires while accumulating
            // the 64 hi-bits — net effect is r = hi.  Skip those 64 iterations.
            ru64 quot = 0;
            ru64 rem = ru64s.hi;

            for (int i = bitsof<ru64>.val - 1; i >= 0; --i) {
                ru64 const bit      = (ru64s.lo >> i) & 1ULL;
                bool const overflow = (rem >> 63) != 0;
                rem = (rem << 1) | bit;
                if (overflow || rem >= divisor) {
                    rem -= divisor;
                    // Note: when overflow=true, r_before < divisor (proved by invariant),
                    // so borrow=1 always.  Unsigned wrap of (r -= divisor) is correct:
                    // the 65th-bit carry and the borrow cancel, leaving r in [0, divisor).
                    quot |= (1ULL << i);  // i ∈ [0, 63] — no UB
                }
            }
            return { .quot = quot, .rem = rem };
        }
    }
    constexpr auto udiv128_64(ru64_pair ru64s, ru64 divisor) noexcept -> u128_div_result
    {
        RAWR_ASSERTION(divisor_is_zero,   divisor != 0);
        RAWR_ASSERTION(div128_overflow,   ru64s.hi < divisor);

        #if RAWR_COMPILER_MSVC
            if (intrin::is_consteval())  { return soft::udiv128_64(ru64s, divisor); }
            ru64 rem  = 0;
            ru64 quot = msvc::_udiv128(ru64s.hi, ru64s.lo, divisor, &rem);
            return { quot, rem };
        #else
            unsigned __int128 num = (static_cast<unsigned __int128>(ru64s.hi) << bitsof<ru64>.val) | ru64s.lo;
            return {
                .quot = static_cast<ru64>(num / divisor),
                .rem  = static_cast<ru64>(num % divisor)
            };
        #endif
    }
}

// Overflow arithmethic.
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
            auto const lhs = static_cast<ruint_of<Raw>>(lhs_);
            auto const rhs = static_cast<ruint_of<Raw>>(rhs_);
            Raw const result = static_cast<Raw>( lhs + rhs );
            if constexpr (uint<Raw>) { return { result, static_cast<ruint_of<Raw>>(lhs + rhs) < lhs }; }
            else                     { return { result, did_add_overflow(lhs_, rhs_, result) }; }
        }

        template <raint Raw>
        [[nodiscard]] constexpr auto ov_sub(Raw lhs_, Raw rhs_) noexcept -> ov_result<Raw>
        {
            auto const lhs = static_cast<ruint_of<Raw>>(lhs_);
            auto const rhs = static_cast<ruint_of<Raw>>(rhs_);
            Raw const result = static_cast<Raw>( lhs - rhs );
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

            ru64 const min_mag = static_cast<ru64>(aint_max<Raw>) + 1ULL;
            bool const overflowed = ret.hi != 0 || ret.lo > min_mag;
            return { static_cast<Raw>(ru64{0} - ret.lo), overflowed };
        }
    }

    template <raint Raw>
    [[nodiscard]] constexpr auto ov_add(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
    {
        #if RAWR_COMPILER_FAMILY_GNU
            Raw result;
            bool const overflowed = __builtin_add_overflow(lhs, rhs, &result);
            return { result, overflowed };
        #elif RAWR_COMPILER_MSVC
            if (intrin::is_consteval()) return soft::ov_add(lhs, rhs);
            if constexpr (sizeof(Raw) == 4)
            {
                unsigned int result_;
                unsigned char carry = msvc::_addcarry_u32(0, static_cast<unsigned int>(lhs), static_cast<unsigned int>(rhs), &result_);
                Raw const result = static_cast<Raw>(result_);
                if constexpr (uint<Raw>) return { result, carry != 0 };
                else return { result, soft::did_add_overflow(lhs, rhs, result) };
            }
            else if constexpr (sizeof(Raw) == 8)
            {
                unsigned __int64 result_;
                unsigned char carry = msvc::_addcarry_u64(0, static_cast<unsigned __int64>(lhs), static_cast<unsigned __int64>(rhs), &result_);
                Raw const result = static_cast<Raw>(result_);
                if constexpr (uint<Raw>) return { result, carry != 0 };
                else return { result, soft::did_add_overflow(lhs, rhs, result) };
            }
            else return soft::ov_add(lhs, rhs);
        #else
            return soft::ov_add(lhs, rhs);
        #endif
    }

    template <raint Raw>
    [[nodiscard]] constexpr auto ov_sub(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
    {
        #if RAWR_COMPILER_FAMILY_GNU
            Raw result;
            bool const overflowed = __builtin_sub_overflow(lhs, rhs, &result);
            return { result, overflowed };
        #elif RAWR_COMPILER_MSVC
            if (intrin::is_consteval()) return soft::ov_sub(lhs, rhs);
                if constexpr (sizeof(Raw) == 4)
                {
                    unsigned int result_;
                    unsigned char carry = msvc::_subborrow_u32(0, static_cast<unsigned int>(lhs), static_cast<unsigned int>(rhs), &result_);
                    Raw const result = static_cast<Raw>(result_);
                    if constexpr (uint<Raw>) return { result, carry != 0 };
                    else return { result, soft::did_sub_underflow(lhs, rhs, result) };
                    }
            else if constexpr (sizeof(Raw) == 8)
            {
                unsigned __int64 result_;
                unsigned char carry = msvc::_subborrow_u64(0, static_cast<unsigned __int64>(lhs), static_cast<unsigned __int64>(rhs), &result_);
                Raw const result = static_cast<Raw>(result_);
                if constexpr (uint<Raw>) return { result, carry != 0 };
                else return { result, soft::did_sub_underflow(lhs, rhs, result) };
            }
            else return soft::ov_sub(lhs, rhs);
        #else
            return soft::ov_sub(lhs, rhs);
        #endif
    }

    template <raint Raw>
    [[nodiscard]] constexpr auto ov_mul(Raw lhs, Raw rhs) noexcept -> ov_result<Raw>
    {
        #if RAWR_COMPILER_FAMILY_GNU
            Raw result;
            bool const overflowed = __builtin_mul_overflow(lhs, rhs, &result);
            return { result, overflowed };
        #elif RAWR_COMPILER_MSVC
            if constexpr (sizeof(Raw) <= 4) return soft::ov_mul(lhs, rhs);
            else {
                if (intrin::is_consteval()) return soft::ov_mul(lhs, rhs);
                if constexpr (uint<Raw>)
                {
                    unsigned __int64 high;
                    unsigned __int64 low = msvc::_umul128(static_cast<unsigned __int64>(lhs), static_cast<unsigned __int64>(rhs), &high);
                    return { static_cast<Raw>(low), high != 0 };
                }
                else
                {
                    __int64 high;
                    __int64 low = msvc::_mul128(static_cast<__int64>(lhs), static_cast<__int64>(rhs), &high);
                    return { static_cast<Raw>(low), high != (low >> 63) };
                }
            }
        #else
            return soft::ov_mul(lhs, rhs);
        #endif
    }
}
