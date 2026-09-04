#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/integer/strong.hpp"
#endif

#include "rawr/lib/dist/todo.pp"

#ifdef RAWR_MODULE
    export module rawr.lib.integer.strong;
    import rawr.lib.integer.base;
    import rawr.lib.integer.raw;
    import rawr.lib.intrin;
    import rawr.lib.bits;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/intrin.hpp"
    #include "rawr/lib/bits.hpp"

    #include "rawr/lib/dist/header.pp"
#endif

namespace rawr::inline lib::inline integer::inline strong::detail
{
    // Do not define these.
    #if RAWR_COMPILER_FAMILY_GNU
        [[gnu::error("Literal underflows target type")]]      void lit_underflows_target_min() noexcept;
        [[gnu::error("Literal overflows target type")]]       void lit_overflows_target_max()  noexcept;
        [[gnu::error("Negative literal to unsigned target")]] void lit_negative_to_unsigned()  noexcept;
    #else
        void lit_underflows_target_min() noexcept;
        void lit_overflows_target_max() noexcept;
        void lit_negative_to_unsigned() noexcept;
    #endif
}

RAWR_EXPORT namespace rawr::inline lib::inline integer::inline strong
{
    RAWR_TODO("Assetion is a dummy for now. Crucial for correcness, need implementation")
    #define RAWR_ASSERTION(...)

    enum class integer_policy : ru8 { checked, wrapping, saturating };

    template <typename Derived, aint RawType, integer_policy Policy = integer_policy::checked>
    struct strong_integer
    {
    private:
        [[nodiscard]] constexpr auto self()       { return static_cast<Derived&>(*this); }
        [[nodiscard]] constexpr auto self() const { return static_cast<Derived const&>(*this); }
    public:
        using raw_type  = RawType;
        using uraw_type = ruint_of<raw_type>;

        raw_type raw = 0;

        /* --- Static Metadata --- */
        static constexpr bool is_signed     = sint<raw_type>;
        static constexpr bool is_unsigned   = uint<raw_type>;
        static constexpr auto bits          = bitsof<raw_type>;
        static constexpr auto policy        = Policy;
        static constexpr auto is_checked    = policy == integer_policy::checked;
        static constexpr auto is_wrapping   = policy == integer_policy::wrapping;
        static constexpr auto is_saturating = policy == integer_policy::saturating;
        // NOTE: These are in raw_type and not in derived_type. Ugly wart from doing CRTP.
        static constexpr auto min         = aint_min<raw_type>;
        static constexpr auto max         = aint_max<raw_type>;

        /* --- Constructors & Factory Helpers --- */
    private:
        constexpr strong_integer() noexcept = default;
        constexpr explicit strong_integer(raw_type val) noexcept : raw{ val } {}
        friend Derived;

    public:
        [[nodiscard]] static constexpr auto narrow(aint auto val)   noexcept -> Derived { RAWR_ASSERTION(val >= min && val <= max); return Derived{ static_cast<raw_type>(val) }; }
        [[nodiscard]] static constexpr auto saturate(aint auto val) noexcept -> Derived { return Derived{ aint_saturating_cast<raw_type>(val) }; }

        /* --- Primitive Cast --- */
        [[nodiscard]] constexpr explicit operator raw_type() const noexcept { return raw; }

        /* --- Comparisons --- */
        constexpr explicit operator bool() = delete;
        [[nodiscard]] friend constexpr auto operator==(Derived lhs, Derived rhs) noexcept -> bool { return lhs.raw == rhs.raw; }
        [[nodiscard]] friend constexpr auto operator!=(Derived lhs, Derived rhs) noexcept -> bool { return lhs.raw != rhs.raw; }
        [[nodiscard]] friend constexpr auto operator< (Derived lhs, Derived rhs) noexcept -> bool { return lhs.raw <  rhs.raw; }
        [[nodiscard]] friend constexpr auto operator> (Derived lhs, Derived rhs) noexcept -> bool { return lhs.raw >  rhs.raw; }
        [[nodiscard]] friend constexpr auto operator<=(Derived lhs, Derived rhs) noexcept -> bool { return lhs.raw <= rhs.raw; }
        [[nodiscard]] friend constexpr auto operator>=(Derived lhs, Derived rhs) noexcept -> bool { return lhs.raw >= rhs.raw; }

        /* --- Unary & Increment / Decrement --- */
        [[nodiscard]] constexpr auto operator+() const noexcept -> Derived { return self(); }

        // Unary minus wraps (two's complement) by doing unsigned negation.
        [[nodiscard]] constexpr auto wrap_neg() const noexcept -> Derived requires (is_signed) { return Derived{ static_cast<raw_type>(uraw_type{0} - static_cast<uraw_type>(raw)) }; }
        [[nodiscard]] constexpr auto chk_neg()  const noexcept -> Derived requires (is_signed) { RAWR_ASSERTION(raw != min); return wrap_neg(); }
        [[nodiscard]] constexpr auto sat_neg()  const noexcept -> Derived requires (is_signed) { return (raw == min) ? Derived{ max } : wrap_neg(); }

        [[nodiscard]] friend constexpr auto operator-(Derived val) noexcept -> Derived requires (is_signed)
        {
                 if constexpr (is_wrapping)   { return val.wrap_neg(); }
            else if constexpr (is_saturating) { return val.sat_neg(); }
            else                              { return val.chk_neg(); }
        }

        [[nodiscard]] constexpr auto operator++(int) noexcept -> Derived  { auto prev = self(); ++raw; return prev; }
        [[nodiscard]] constexpr auto operator--(int) noexcept -> Derived  { auto prev = self(); --raw; return prev; }
                      constexpr auto operator++()    noexcept -> Derived& { ++raw; return self(); }
                      constexpr auto operator--()    noexcept -> Derived& { --raw; return self(); }

        /* --- Overflow query --- */
        [[nodiscard]] constexpr auto ov_add(Derived rhs) const noexcept -> intrin::ov_result<Derived> { auto ret = intrin::ov_add(raw, rhs.raw); return { Derived{ret.val}, ret.overflowed}; }
        [[nodiscard]] constexpr auto ov_sub(Derived rhs) const noexcept -> intrin::ov_result<Derived> { auto ret = intrin::ov_sub(raw, rhs.raw); return { Derived{ret.val}, ret.overflowed}; }
        [[nodiscard]] constexpr auto ov_mul(Derived rhs) const noexcept -> intrin::ov_result<Derived> { auto ret = intrin::ov_mul(raw, rhs.raw); return { Derived{ret.val}, ret.overflowed}; }

        /* --- Wrapping arithmetic --- */
        [[nodiscard]] constexpr auto wrap_add(Derived rhs) const noexcept -> Derived { return Derived{ static_cast<raw_type>( static_cast<uraw_type>(raw) + static_cast<uraw_type>(rhs.raw) ) }; }
        [[nodiscard]] constexpr auto wrap_sub(Derived rhs) const noexcept -> Derived { return Derived{ static_cast<raw_type>( static_cast<uraw_type>(raw) - static_cast<uraw_type>(rhs.raw) ) }; }
        [[nodiscard]] constexpr auto wrap_mul(Derived rhs) const noexcept -> Derived { return Derived{ static_cast<raw_type>( static_cast<uraw_type>(raw) * static_cast<uraw_type>(rhs.raw) ) }; }

        /* --- Checked arithmetic: asserts on overflow; hard compile error if consteval. --- */
        [[nodiscard]] constexpr auto chk_add(Derived rhs) const noexcept -> Derived
        {
            auto const ret = ov_add(rhs);
            if (intrin::is_consteval()) { if (ret.overflowed) { detail::lit_overflows_target_max(); } }
            else                        { RAWR_ASSERTION(!ret.overflowed); }
            return ret.val;
        }
        [[nodiscard]] constexpr auto chk_sub(Derived rhs) const noexcept -> Derived
        {
            auto const ret = ov_sub(rhs);
            if (intrin::is_consteval()) { if (ret.overflowed) { detail::lit_underflows_target_min(); } }
            else                        { RAWR_ASSERTION(!ret.overflowed); }
            return ret.val;
        }
        [[nodiscard]] constexpr auto chk_mul(Derived rhs) const noexcept -> Derived
        {
            auto const ret = ov_mul(rhs);
            if (intrin::is_consteval()) { if (ret.overflowed) { detail::lit_overflows_target_max(); } }
            else                        { RAWR_ASSERTION(!ret.overflowed); }
            return ret.val;
        }

        /* --- Saturating arithmetic: always defined, clamps. --- */
        [[nodiscard]] constexpr auto sat_add(Derived rhs) const noexcept -> Derived
        {
            auto const ret = ov_add(rhs);
            if (!ret.overflowed) { return ret.val; }
            if constexpr (is_unsigned) { return Derived{ max }; }
            else { return (raw > 0) ? Derived{ max } : Derived{ min }; }
        }
        [[nodiscard]] constexpr auto sat_sub(Derived rhs) const noexcept -> Derived
        {
            auto const ret = ov_sub(rhs);
            if (!ret.overflowed) { return ret.val; }
            if constexpr (is_unsigned) { return Derived{ min }; }
            else { return (raw >= 0) ? Derived{ max } : Derived{ min }; }
        }
        [[nodiscard]] constexpr auto sat_mul(Derived rhs) const noexcept -> Derived
        {
            auto const ret = ov_mul(rhs);
            if (!ret.overflowed) { return ret.val; }
            if constexpr (is_unsigned) { return Derived{ max }; }
            else { return ((raw < 0) == (rhs.raw < 0)) ? Derived{ max } : Derived{ min }; }
        }

        [[nodiscard]] constexpr auto sat_div(Derived rhs) const noexcept -> Derived
        {
            RAWR_ASSERTION(rhs.raw != raw_type{0});
            if constexpr (is_signed) { if (raw == min && rhs.raw == raw_type{-1}) { return Derived{ max }; } }
            return Derived{ static_cast<raw_type>(raw / rhs.raw) };
        }

        [[nodiscard]] constexpr auto sat_rem(Derived rhs) const noexcept -> Derived
        {
            RAWR_ASSERTION(rhs.raw != raw_type{0});
            if constexpr (is_signed) { if (raw == min && rhs.raw == raw_type{-1}) { return Derived{ 0 }; } }
            return Derived{ static_cast<raw_type>(raw % rhs.raw) };
        }

        /* --- Default operators: dispatch on Policy via if constexpr — only
               the selected branch is ever instantiated. --- */
        [[nodiscard]] friend constexpr auto operator+(Derived lhs, Derived rhs) noexcept -> Derived
        {
                 if constexpr (is_wrapping)   { return lhs.wrap_add(rhs); }
            else if constexpr (is_saturating) { return lhs.sat_add(rhs); }
            else                              { return lhs.chk_add(rhs); }
        }
        [[nodiscard]] friend constexpr auto operator-(Derived lhs, Derived rhs) noexcept -> Derived
        {
                 if constexpr (is_wrapping)   { return lhs.wrap_sub(rhs); }
            else if constexpr (is_saturating) { return lhs.sat_sub(rhs); }
            else                              { return lhs.chk_sub(rhs); }
        }
        [[nodiscard]] friend constexpr auto operator*(Derived lhs, Derived rhs) noexcept -> Derived
        {
                 if constexpr (is_wrapping)   { return lhs.wrap_mul(rhs); }
            else if constexpr (is_saturating) { return lhs.sat_mul(rhs); }
            else                              { return lhs.chk_mul(rhs); }
        }

        // No sensible "wrapping" division exists — unconditional regardless of Policy.
        [[nodiscard]] constexpr auto operator/(Derived rhs) const noexcept -> Derived
        {
            RAWR_ASSERTION(rhs.raw != raw_type{0});
            if constexpr (is_signed) { RAWR_ASSERTION(!(raw == min && rhs.raw == raw_type{-1})); }
            return Derived{ static_cast<raw_type>(raw / rhs.raw) };
        }
        [[nodiscard]] constexpr auto operator%(Derived rhs) const noexcept -> Derived
        {
            RAWR_ASSERTION(rhs.raw != raw_type{0});
            if constexpr (is_signed) { RAWR_ASSERTION(!(raw == min && rhs.raw == raw_type{-1})); }
            return Derived{ static_cast<raw_type>(raw % rhs.raw) };
        }


        constexpr auto operator+=(Derived rhs) noexcept -> Derived& { *this = self() + rhs; return self(); }
        constexpr auto operator-=(Derived rhs) noexcept -> Derived& { *this = self() - rhs; return self(); }
        constexpr auto operator*=(Derived rhs) noexcept -> Derived& { *this = self() * rhs; return self(); }
        constexpr auto operator/=(Derived rhs) noexcept -> Derived& { *this = self() / rhs; return self(); }
        constexpr auto operator%=(Derived rhs) noexcept -> Derived& { *this = self() % rhs; return self(); }

        /* --- Bitwise Ops --- */
        RAWR_TODO("Review the shifts. Needs care for the type of the shifter, like loom.a.ne")
        [[nodiscard]] constexpr auto operator~() const noexcept -> Derived { return Derived{ static_cast<raw_type>(~raw) }; }
        [[nodiscard]] friend constexpr auto operator&(Derived lhs, Derived rhs)  noexcept -> Derived { return Derived{ static_cast<raw_type>(lhs.raw & rhs.raw) }; }
        [[nodiscard]] friend constexpr auto operator|(Derived lhs, Derived rhs)  noexcept -> Derived { return Derived{ static_cast<raw_type>(lhs.raw | rhs.raw) }; }
        [[nodiscard]] friend constexpr auto operator^(Derived lhs, Derived rhs)  noexcept -> Derived { return Derived{ static_cast<raw_type>(lhs.raw ^ rhs.raw) }; }
        [[nodiscard]] friend constexpr auto operator<<(Derived lhs, Derived rhs) noexcept -> Derived { return static_cast<uraw_type>(rhs.raw) >= static_cast<uraw_type>(bits) ? Derived{0} : Derived{ static_cast<raw_type>(lhs.raw << rhs.raw) }; }
        [[nodiscard]] friend constexpr auto operator>>(Derived lhs, Derived rhs) noexcept -> Derived { return static_cast<uraw_type>(rhs.raw) >= static_cast<uraw_type>(bits) ? Derived{0} : Derived{ static_cast<raw_type>(lhs.raw >> rhs.raw) }; }
        constexpr auto operator&=(Derived rhs)  noexcept -> Derived& { *this = self() &  rhs; return self(); }
        constexpr auto operator|=(Derived rhs)  noexcept -> Derived& { *this = self() |  rhs; return self(); }
        constexpr auto operator^=(Derived rhs)  noexcept -> Derived& { *this = self() ^  rhs; return self(); }
        constexpr auto operator<<=(Derived rhs) noexcept -> Derived& { *this = self() << rhs; return self(); }
        constexpr auto operator>>=(Derived rhs) noexcept -> Derived& { *this = self() >> rhs; return self(); }

        /* --- Utilities --- */
        [[nodiscard]] constexpr auto popcount()       const noexcept -> Derived { return Derived{ intrin::popcount(raw) };  }
        [[nodiscard]] constexpr auto leading_zeros()  const noexcept -> Derived { return Derived{ intrin::leading_zeros(raw) };  }
        [[nodiscard]] constexpr auto trailing_zeros() const noexcept -> Derived { return Derived{ intrin::trailing_zeros(raw) };  }
        [[nodiscard]] constexpr auto leading_ones()   const noexcept -> Derived { return Derived{ intrin::leading_ones(raw) };  }
        [[nodiscard]] constexpr auto trailing_ones()  const noexcept -> Derived { return Derived{ intrin::trailing_ones(raw) };  }

        [[nodiscard]] constexpr auto rotl(Derived n) const noexcept -> Derived
        requires (is_unsigned)
        {
            uraw_type const rem = static_cast<uraw_type>(n.raw) % static_cast<uraw_type>(bits);

            if (rem == uraw_type{0}) { return self(); }

            return Derived{
                static_cast<raw_type>(
                    (static_cast<uraw_type>(raw) << rem) |
                    (static_cast<uraw_type>(raw) >> (bits - rem))
                )
            };
        }
        [[nodiscard]] constexpr auto rotr(Derived n) const noexcept -> Derived
        requires (is_unsigned)
        {
            uraw_type const rem = static_cast<uraw_type>(n.raw) % static_cast<uraw_type>(bits);

            if (rem == uraw_type{0}) { return self(); }

            return Derived{
                static_cast<raw_type>(
                    (static_cast<uraw_type>(raw) >> rem) |
                    (static_cast<uraw_type>(raw) << (bits - rem))
                )
            };
        }

        [[nodiscard]] constexpr auto is_power_of_two()   const noexcept -> bool
        requires (is_unsigned)
        { return raw != raw_type{0} && (raw & (raw - raw_type{1})) == raw_type{0}; }
        [[nodiscard]] constexpr auto next_power_of_two() const noexcept -> Derived
        requires (is_unsigned)
        {
            RAWR_ASSERTION(raw <= static_cast<raw_type>(uraw_type{1} << (bits - 1u)));

            if (raw <= raw_type{1}) { return Derived{ raw_type{1} }; }
            return Derived{raw_type{1} << (bits.val - (*this - Derived{1}).leading_zeros())};
        }
        [[nodiscard]] constexpr auto prev_power_of_two() const noexcept -> Derived
        requires (is_unsigned)
        {
            if (raw == raw_type{0}) { return Derived{ raw_type{0} }; };
            return Derived{raw_type{1} << (bits.val - 1 - leading_zeros(raw))};
        }

        [[nodiscard]] constexpr auto is_between(Derived low, Derived high)           const noexcept -> bool { return raw >= low.raw && raw <  high.raw; }
        [[nodiscard]] constexpr auto is_between_inclusive(Derived low, Derived high) const noexcept -> bool { return raw >= low.raw && raw <= high.raw; }
        [[nodiscard]] constexpr auto is_between_exclusive(Derived low, Derived high) const noexcept -> bool { return raw >  low.raw && raw <  high.raw; }

        [[nodiscard]] constexpr auto clamp(Derived low, Derived high) const noexcept -> Derived
        {
            RAWR_ASSERTION(low.raw <= high.raw);

            if (raw < low.raw)  { return low; }
            if (raw > high.raw) { return high; }
            return self();
        }

        /* --- abs(): dispatches on Policy, mirroring unary minus. wrap_neg
               already exists above — reuse it instead of duplicating the
               unsigned-negation trick. --- */
        [[nodiscard]] constexpr auto wrap_abs() const noexcept -> Derived requires (is_signed) { return (raw >= raw_type{0}) ? self() : wrap_neg(); }
        [[nodiscard]] constexpr auto chk_abs()  const noexcept -> Derived requires (is_signed) { RAWR_ASSERTION(raw != min); return wrap_abs(); }
        [[nodiscard]] constexpr auto sat_abs()  const noexcept -> Derived requires (is_signed) { return (raw == min) ? Derived{ max } : wrap_abs(); }
        [[nodiscard]] constexpr auto abs()      const noexcept -> Derived requires (is_signed)
        {
                 if constexpr (is_wrapping)   { return wrap_abs(); }
            else if constexpr (is_saturating) { return sat_abs(); }
            else                              { return chk_abs(); }
        }

        [[nodiscard]] constexpr auto wrap(Derived low, Derived high) const noexcept -> Derived
        {
            RAWR_ASSERTION(high.raw > low.raw);
            if constexpr (bits < biw64) {
                using wsraw_type = rsint_exact<bitwidth{ bitsof<raw_type>.val * 2 }>;
                wsraw_type const range  = static_cast<wsraw_type>(high.raw) - static_cast<wsraw_type>(low.raw);
                wsraw_type const offset = static_cast<wsraw_type>(raw)      - static_cast<wsraw_type>(low.raw);
                wsraw_type const result = ((offset % range) + range) % range;
                return Derived{ static_cast<raw_type>(static_cast<wsraw_type>(low.raw) + result) };
            } else {
                // bits == 64 – avoid wsraw_type; use sign/magnitude with 64‑bit modulo
                if constexpr (is_unsigned) {
                    const ru64 range = static_cast<ru64>(high.raw - low.raw);
                    const bool neg = (raw < low.raw);
                    const ru64 mag = neg ? static_cast<ru64>(low.raw - raw) : static_cast<ru64>(raw - low.raw);
                    const ru64 rem = mag % range;
                    const ru64 wrapped = (!neg || rem == 0) ? rem : range - rem;
                    return Derived{ static_cast<raw_type>(low.raw + wrapped) };
                } else {
                    // signed 64‑bit wrap: use unsigned representation and then cast
                    const auto uraw_r = static_cast<uraw_type>(raw);
                    const auto uraw_l = static_cast<uraw_type>(low.raw);
                    const auto range  = static_cast<uraw_type>(high.raw - low.raw);
                    const bool neg = (raw < low.raw);
                    const uraw_type mag = neg ? uraw_l - uraw_r : uraw_r - uraw_l;
                    const uraw_type rem = mag % range;
                    const uraw_type wrapped = (!neg || rem == 0) ? rem : range - rem;
                    return Derived{ static_cast<raw_type>(static_cast<uraw_type>(low.raw) + wrapped) };
                }
            }
        }
        [[nodiscard]] constexpr auto wrap(Derived high) const noexcept -> Derived
        {
            RAWR_ASSERTION(high.raw != raw_type{0});
            if constexpr (bits < biw64) {
                if constexpr (is_unsigned) { return Derived{ static_cast<raw_type>(raw % high.raw) }; }
                else {
                    using wsraw_type = rsint_exact<bitwidth{ bitsof<raw_type>.val * 2 }>;

                    return Derived{ static_cast<raw_type>(
                        (
                            (static_cast<wsraw_type>(raw) % static_cast<wsraw_type>(high.raw)) +
                            static_cast<wsraw_type>(high.raw)
                        ) %
                            static_cast<wsraw_type>(high.raw)
                    ) };
                }
            } else {
                // bits == 64
                if constexpr (is_unsigned) {
                    return Derived{ static_cast<raw_type>(raw % high.raw) };
                } else {
                    // signed wrap with positive/negative modulus
                    const uraw_type modulus = high.raw < 0 ? uraw_type{0} - static_cast<uraw_type>(high.raw) : static_cast<uraw_type>(high.raw);
                    const uraw_type value   = raw < 0 ? uraw_type{0} - static_cast<uraw_type>(raw) : static_cast<uraw_type>(raw);
                    const bool neg = (raw < 0);
                    uraw_type rem = value % modulus;
                    if (rem != 0 && neg) { rem = modulus - rem; }
                    return Derived{ static_cast<raw_type>(rem) };
                }
            }
        }

        [[nodiscard]] constexpr auto div_floor(Derived rhs) const noexcept -> Derived
        {
            RAWR_ASSERTION(rhs.raw != raw_type{0});

            if constexpr (is_signed) { RAWR_ASSERTION(!(raw == min && rhs.raw == raw_type{-1})); }

            if constexpr (is_unsigned) { return Derived{ static_cast<raw_type>(raw / rhs.raw) }; }
            else
            {
                auto const quot = raw / rhs.raw;
                auto const rem  = raw % rhs.raw;
                return Derived{ static_cast<raw_type>(quot - (rem != 0 && ((rem ^ rhs.raw) < 0))) };
            }
        }

        [[nodiscard]] constexpr auto div_ceil(Derived rhs) const noexcept -> Derived
        {
            RAWR_ASSERTION(rhs.raw != raw_type{0});

            if constexpr (is_signed) { RAWR_ASSERTION(!(raw == min && rhs.raw == raw_type{-1})); }

            if constexpr (is_unsigned) { return Derived{ static_cast<raw_type>((raw / rhs.raw) + (raw % rhs.raw != raw_type{0})) }; }
            else
            {
                auto const quot = raw / rhs.raw;
                auto const rem  = raw % rhs.raw;
                return Derived{ static_cast<raw_type>(quot + (rem != 0 && ((rem ^ rhs.raw) >= 0))) };
            }
        }
    };

    /// First we forward-declare all the types involved. This needs to be done as u8 directly references s64 and so on.

    namespace checked    { struct u8; struct u16; struct u32; struct u64; struct s8; struct s16; struct s32; struct s64; }
    namespace wrapping   { struct u8; struct u16; struct u32; struct u64; struct s8; struct s16; struct s32; struct s64; }
    namespace saturating { struct u8; struct u16; struct u32; struct u64; struct s8; struct s16; struct s32; struct s64; }

    /// Then we actually go about declaring each one and whatever conversions are suitable.

    #define RAWR_LIB_INTEGER_DECLARE(Name, Raw, Policy, ...)                                                  \
        struct Policy::Name : strong_integer<Policy::Name, Raw, integer_policy::Policy>                       \
        {                                                                                                     \
            constexpr Name() = default;                                                                       \
            constexpr explicit Name(raw_type val) noexcept : strong_integer{ val } {}                                    \
            consteval Name(aint auto val) noexcept : strong_integer{ rawr::aint_from_literal<raw_type>(val) } {}         \
                                                                                                              \
            using checked    = checked::Name;                                                                 \
            using wrapping   = wrapping::Name;                                                                \
            using saturating = saturating::Name;                                                              \
            [[nodiscard]] constexpr auto as_checked()    const noexcept -> checked;                           \
            [[nodiscard]] constexpr auto as_wrapping()   const noexcept -> wrapping;                          \
            [[nodiscard]] constexpr auto as_saturating() const noexcept -> saturating;                        \
            __VA_ARGS__                                                                                       \
        };                                                                                                    \
        static_assert(                                                                                        \
            intrin::is_trivially_copyable<Policy::Name> &&                                                    \
            intrin::is_standard_layout<Policy::Name> &&                                                       \
            sizeof(Policy::Name)  == sizeof(Raw) &&                                                           \
            alignof(Policy::Name) == alignof(Raw),                                                            \
            "strong integer must be layout-compatible with its raw type")
    #define RAWR_LIB_INTEGER_DECLARE3(Name, Raw, ConversionMacro)                      \
        RAWR_LIB_INTEGER_DECLARE(Name, Raw,  saturating, ConversionMacro(saturating)); \
        RAWR_LIB_INTEGER_DECLARE(Name, Raw,  wrapping,   ConversionMacro(wrapping));   \
        RAWR_LIB_INTEGER_DECLARE(Name, Raw,  checked,    ConversionMacro(checked));

    // Per-width conversion-declaration macro, parameterized on target namespace,
    // so checked/wrapping/saturating each get the identical, policy-preserving
    // surface. One of these per width (u8..s64).
    #define RAWR_LIB_INTEGER_U8_CONVERSIONS(NS)                                                        \
        [[nodiscard]] constexpr auto u16()                          const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u32()                          const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u64()                          const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto s16()                          const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s32()                          const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s64()                          const noexcept -> strong::NS::s64; \
        [[nodiscard]] constexpr explicit operator strong::NS::u16() const noexcept;                    \
        [[nodiscard]] constexpr explicit operator strong::NS::u32() const noexcept;                    \
        [[nodiscard]] constexpr explicit operator strong::NS::u64() const noexcept;                    \
        [[nodiscard]] constexpr explicit operator strong::NS::s16() const noexcept;                    \
        [[nodiscard]] constexpr explicit operator strong::NS::s32() const noexcept;                    \
        [[nodiscard]] constexpr explicit operator strong::NS::s64() const noexcept;                    \
        [[nodiscard]] constexpr auto s8_unchecked()                 const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_narrow()                    const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_saturate()                  const noexcept -> strong::NS::s8;
    #define RAWR_LIB_INTEGER_U16_CONVERSIONS(NS)                                                 \
        [[nodiscard]] constexpr auto u32()                    const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u64()                    const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto s32()                    const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s64()                    const noexcept -> strong::NS::s64; \
        [[nodiscard]] constexpr explicit operator strong::NS::u32() const noexcept;              \
        [[nodiscard]] constexpr explicit operator strong::NS::u64() const noexcept;              \
        [[nodiscard]] constexpr explicit operator strong::NS::s32() const noexcept;              \
        [[nodiscard]] constexpr explicit operator strong::NS::s64() const noexcept;              \
        [[nodiscard]] constexpr auto u8_unchecked()           const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_narrow()              const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_saturate()            const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto s8_unchecked()           const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_narrow()              const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_saturate()            const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s16_unchecked()          const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s16_narrow()             const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s16_saturate()           const noexcept -> strong::NS::s16;
    #define RAWR_LIB_INTEGER_U32_CONVERSIONS(NS)                                                 \
        [[nodiscard]] constexpr auto u64()                    const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto s64()                    const noexcept -> strong::NS::s64; \
        [[nodiscard]] constexpr explicit operator strong::NS::u64() const noexcept;              \
        [[nodiscard]] constexpr explicit operator strong::NS::s64() const noexcept;              \
        [[nodiscard]] constexpr auto u8_unchecked()           const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_narrow()              const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_saturate()            const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u16_unchecked()          const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_narrow()             const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_saturate()           const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto s8_unchecked()           const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_narrow()              const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_saturate()            const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s16_unchecked()          const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s16_narrow()             const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s16_saturate()           const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s32_unchecked()          const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s32_narrow()             const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s32_saturate()           const noexcept -> strong::NS::s32;
    #define RAWR_LIB_INTEGER_U64_CONVERSIONS(NS)                                                 \
        [[nodiscard]] constexpr auto u8_unchecked()           const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_narrow()              const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_saturate()            const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u16_unchecked()          const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_narrow()             const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_saturate()           const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u32_unchecked()          const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u32_narrow()             const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u32_saturate()           const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto s8_unchecked()           const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_narrow()              const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_saturate()            const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s16_unchecked()          const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s16_narrow()             const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s16_saturate()           const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s32_unchecked()          const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s32_narrow()             const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s32_saturate()           const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s64_unchecked()          const noexcept -> strong::NS::s64; \
        [[nodiscard]] constexpr auto s64_narrow()             const noexcept -> strong::NS::s64; \
        [[nodiscard]] constexpr auto s64_saturate()           const noexcept -> strong::NS::s64;
    #define RAWR_LIB_INTEGER_S8_CONVERSIONS(NS)                                                  \
        [[nodiscard]] constexpr auto s16()                    const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s32()                    const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s64()                    const noexcept -> strong::NS::s64; \
        [[nodiscard]] constexpr explicit operator strong::NS::s16() const noexcept;              \
        [[nodiscard]] constexpr explicit operator strong::NS::s32() const noexcept;              \
        [[nodiscard]] constexpr explicit operator strong::NS::s64() const noexcept;              \
        [[nodiscard]] constexpr auto u8_unchecked()           const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_narrow()              const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_saturate()            const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u16_unchecked()          const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_narrow()             const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_saturate()           const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u32_unchecked()          const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u32_narrow()             const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u32_saturate()           const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u64_unchecked()          const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto u64_narrow()             const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto u64_saturate()           const noexcept -> strong::NS::u64;
    #define RAWR_LIB_INTEGER_S16_CONVERSIONS(NS)                                                 \
        [[nodiscard]] constexpr auto s32()                    const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s64()                    const noexcept -> strong::NS::s64; \
        [[nodiscard]] constexpr explicit operator strong::NS::s32() const noexcept;              \
        [[nodiscard]] constexpr explicit operator strong::NS::s64() const noexcept;              \
        [[nodiscard]] constexpr auto u8_unchecked()           const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_narrow()              const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_saturate()            const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u16_unchecked()          const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_narrow()             const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_saturate()           const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u32_unchecked()          const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u32_narrow()             const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u32_saturate()           const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u64_unchecked()          const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto u64_narrow()             const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto u64_saturate()           const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto s8_unchecked()           const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_narrow()              const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_saturate()            const noexcept -> strong::NS::s8;
    #define RAWR_LIB_INTEGER_S32_CONVERSIONS(NS)                                                 \
        [[nodiscard]] constexpr auto s64()                    const noexcept -> strong::NS::s64; \
        [[nodiscard]] constexpr explicit operator strong::NS::s64() const noexcept;              \
        [[nodiscard]] constexpr auto u8_unchecked()           const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_narrow()              const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_saturate()            const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u16_unchecked()          const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_narrow()             const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_saturate()           const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u32_unchecked()          const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u32_narrow()             const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u32_saturate()           const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u64_unchecked()          const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto u64_narrow()             const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto u64_saturate()           const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto s8_unchecked()           const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_narrow()              const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_saturate()            const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s16_unchecked()          const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s16_narrow()             const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s16_saturate()           const noexcept -> strong::NS::s16;
    #define RAWR_LIB_INTEGER_S64_CONVERSIONS(NS)                                                 \
        [[nodiscard]] constexpr auto u8_unchecked()           const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_narrow()              const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u8_saturate()            const noexcept -> strong::NS::u8;  \
        [[nodiscard]] constexpr auto u16_unchecked()          const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_narrow()             const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u16_saturate()           const noexcept -> strong::NS::u16; \
        [[nodiscard]] constexpr auto u32_unchecked()          const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u32_narrow()             const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u32_saturate()           const noexcept -> strong::NS::u32; \
        [[nodiscard]] constexpr auto u64_unchecked()          const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto u64_narrow()             const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto u64_saturate()           const noexcept -> strong::NS::u64; \
        [[nodiscard]] constexpr auto s8_unchecked()           const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_narrow()              const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s8_saturate()            const noexcept -> strong::NS::s8;  \
        [[nodiscard]] constexpr auto s16_unchecked()          const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s16_narrow()             const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s16_saturate()           const noexcept -> strong::NS::s16; \
        [[nodiscard]] constexpr auto s32_unchecked()          const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s32_narrow()             const noexcept -> strong::NS::s32; \
        [[nodiscard]] constexpr auto s32_saturate()           const noexcept -> strong::NS::s32;

    RAWR_LIB_INTEGER_DECLARE3(u8,  ru8,  RAWR_LIB_INTEGER_U8_CONVERSIONS);
    RAWR_LIB_INTEGER_DECLARE3(u16, ru16, RAWR_LIB_INTEGER_U16_CONVERSIONS);
    RAWR_LIB_INTEGER_DECLARE3(u32, ru32, RAWR_LIB_INTEGER_U32_CONVERSIONS);
    RAWR_LIB_INTEGER_DECLARE3(u64, ru64, RAWR_LIB_INTEGER_U64_CONVERSIONS);
    RAWR_LIB_INTEGER_DECLARE3(s8,  rs8,  RAWR_LIB_INTEGER_S8_CONVERSIONS);
    RAWR_LIB_INTEGER_DECLARE3(s16, rs16, RAWR_LIB_INTEGER_S16_CONVERSIONS);
    RAWR_LIB_INTEGER_DECLARE3(s32, rs32, RAWR_LIB_INTEGER_S32_CONVERSIONS);
    RAWR_LIB_INTEGER_DECLARE3(s64, rs64, RAWR_LIB_INTEGER_S64_CONVERSIONS);

    #undef RAWR_LIB_INTEGER_U8_CONVERSIONS
    #undef RAWR_LIB_INTEGER_U16_CONVERSIONS
    #undef RAWR_LIB_INTEGER_U32_CONVERSIONS
    #undef RAWR_LIB_INTEGER_U64_CONVERSIONS
    #undef RAWR_LIB_INTEGER_S8_CONVERSIONS
    #undef RAWR_LIB_INTEGER_S16_CONVERSIONS
    #undef RAWR_LIB_INTEGER_S32_CONVERSIONS
    #undef RAWR_LIB_INTEGER_S64_CONVERSIONS
    #undef RAWR_LIB_INTEGER_DECLARE3
    #undef RAWR_LIB_INTEGER_DECLARE

    /// Now we define the out-of-line conversions and outstanding methods.

    #define RAWR_LIB_INTEGER_DEFINE3(DefinitionMacro) \
        DefinitionMacro(saturating)                   \
        DefinitionMacro(wrapping)                     \
        DefinitionMacro(checked)

    #define RAWR_LIB_INTEGER_U8_DEFINITIONS(NS)                                                                                                    \
        constexpr auto NS::u8::as_checked()                const noexcept -> checked    { return checked{ raw }; }                                 \
        constexpr auto NS::u8::as_wrapping()               const noexcept -> wrapping   { return wrapping{ raw }; }                                \
        constexpr auto NS::u8::as_saturating()             const noexcept -> saturating { return saturating{ raw }; }                              \
        constexpr auto NS::u8::u16()                       const noexcept -> strong::NS::u16 { return strong::NS::u16{ static_cast<ru16>(raw) }; } \
        constexpr auto NS::u8::u32()                       const noexcept -> strong::NS::u32 { return strong::NS::u32{ static_cast<ru32>(raw) }; } \
        constexpr auto NS::u8::u64()                       const noexcept -> strong::NS::u64 { return strong::NS::u64{ static_cast<ru64>(raw) }; } \
        constexpr auto NS::u8::s16()                       const noexcept -> strong::NS::s16 { return strong::NS::s16{ static_cast<rs16>(raw) }; } \
        constexpr auto NS::u8::s32()                       const noexcept -> strong::NS::s32 { return strong::NS::s32{ static_cast<rs32>(raw) }; } \
        constexpr auto NS::u8::s64()                       const noexcept -> strong::NS::s64 { return strong::NS::s64{ static_cast<rs64>(raw) }; } \
        constexpr      NS::u8::operator strong::NS::u16()  const noexcept { return this->u16(); }                                                  \
        constexpr      NS::u8::operator strong::NS::u32()  const noexcept { return this->u32(); }                                                  \
        constexpr      NS::u8::operator strong::NS::u64()  const noexcept { return this->u64(); }                                                  \
        constexpr      NS::u8::operator strong::NS::s16()  const noexcept { return this->s16(); }                                                  \
        constexpr      NS::u8::operator strong::NS::s32()  const noexcept { return this->s32(); }                                                  \
        constexpr      NS::u8::operator strong::NS::s64()  const noexcept { return this->s64(); }                                                  \
        constexpr auto NS::u8::s8_unchecked()              const noexcept -> strong::NS::s8 { return strong::NS::s8{ static_cast<rs8>(raw) }; }    \
        constexpr auto NS::u8::s8_narrow()                 const noexcept -> strong::NS::s8 { return strong::NS::s8::narrow(raw); }                \
        constexpr auto NS::u8::s8_saturate()               const noexcept -> strong::NS::s8 { return strong::NS::s8::saturate(raw); }
    #define RAWR_LIB_INTEGER_U16_DEFINITIONS(NS) \
        constexpr auto NS::u16::as_checked()               const noexcept -> checked    { return checked{ raw }; }                                 \
        constexpr auto NS::u16::as_wrapping()              const noexcept -> wrapping   { return wrapping{ raw }; }                                \
        constexpr auto NS::u16::as_saturating()            const noexcept -> saturating { return saturating{ raw }; }                              \
        constexpr auto NS::u16::u32()                      const noexcept -> strong::NS::u32 { return strong::NS::u32{ static_cast<ru32>(raw) }; } \
        constexpr auto NS::u16::u64()                      const noexcept -> strong::NS::u64 { return strong::NS::u64{ static_cast<ru64>(raw) }; } \
        constexpr auto NS::u16::s32()                      const noexcept -> strong::NS::s32 { return strong::NS::s32{ static_cast<rs32>(raw) }; } \
        constexpr auto NS::u16::s64()                      const noexcept -> strong::NS::s64 { return strong::NS::s64{ static_cast<rs64>(raw) }; } \
        constexpr      NS::u16::operator strong::NS::u32() const noexcept { return this->u32(); }                                                  \
        constexpr      NS::u16::operator strong::NS::u64() const noexcept { return this->u64(); }                                                  \
        constexpr      NS::u16::operator strong::NS::s32() const noexcept { return this->s32(); }                                                  \
        constexpr      NS::u16::operator strong::NS::s64() const noexcept { return this->s64(); }                                                  \
        constexpr auto NS::u16::u8_unchecked()             const noexcept -> strong::NS::u8  { return strong::NS::u8{ static_cast<ru8>(raw) }; }   \
        constexpr auto NS::u16::u8_narrow()                const noexcept -> strong::NS::u8  { return strong::NS::u8::narrow(raw); }               \
        constexpr auto NS::u16::u8_saturate()              const noexcept -> strong::NS::u8  { return strong::NS::u8::saturate(raw); }             \
        constexpr auto NS::u16::s8_unchecked()             const noexcept -> strong::NS::s8  { return strong::NS::s8{ static_cast<rs8>(raw) }; }   \
        constexpr auto NS::u16::s8_narrow()                const noexcept -> strong::NS::s8  { return strong::NS::s8::narrow(raw); }               \
        constexpr auto NS::u16::s8_saturate()              const noexcept -> strong::NS::s8  { return strong::NS::s8::saturate(raw); }             \
        constexpr auto NS::u16::s16_unchecked()            const noexcept -> strong::NS::s16 { return strong::NS::s16{ static_cast<rs16>(raw) }; } \
        constexpr auto NS::u16::s16_narrow()               const noexcept -> strong::NS::s16 { return strong::NS::s16::narrow(raw); }              \
        constexpr auto NS::u16::s16_saturate()             const noexcept -> strong::NS::s16 { return strong::NS::s16::saturate(raw); }
    #define RAWR_LIB_INTEGER_U32_DEFINITIONS(NS)\
        constexpr auto NS::u32::as_checked()               const noexcept -> checked    { return checked{ raw }; }                                 \
        constexpr auto NS::u32::as_wrapping()              const noexcept -> wrapping   { return wrapping{ raw }; }                                \
        constexpr auto NS::u32::as_saturating()            const noexcept -> saturating { return saturating{ raw }; }                              \
        constexpr auto NS::u32::u64()                      const noexcept -> strong::NS::u64 { return strong::NS::u64{ static_cast<ru64>(raw) }; } \
        constexpr auto NS::u32::s64()                      const noexcept -> strong::NS::s64 { return strong::NS::s64{ static_cast<rs64>(raw) }; } \
        constexpr      NS::u32::operator strong::NS::u64() const noexcept { return this->u64(); }                                                  \
        constexpr      NS::u32::operator strong::NS::s64() const noexcept { return this->s64(); }                                                  \
        constexpr auto NS::u32::u8_unchecked()             const noexcept -> strong::NS::u8  { return strong::NS::u8{ static_cast<ru8>(raw) }; }   \
        constexpr auto NS::u32::u8_narrow()                const noexcept -> strong::NS::u8  { return strong::NS::u8::narrow(raw); }               \
        constexpr auto NS::u32::u8_saturate()              const noexcept -> strong::NS::u8  { return strong::NS::u8::saturate(raw); }             \
        constexpr auto NS::u32::u16_unchecked()            const noexcept -> strong::NS::u16 { return strong::NS::u16{ static_cast<ru16>(raw) }; } \
        constexpr auto NS::u32::u16_narrow()               const noexcept -> strong::NS::u16 { return strong::NS::u16::narrow(raw); }              \
        constexpr auto NS::u32::u16_saturate()             const noexcept -> strong::NS::u16 { return strong::NS::u16::saturate(raw); }            \
        constexpr auto NS::u32::s8_unchecked()             const noexcept -> strong::NS::s8  { return strong::NS::s8{ static_cast<rs8>(raw) }; }   \
        constexpr auto NS::u32::s8_narrow()                const noexcept -> strong::NS::s8  { return strong::NS::s8::narrow(raw); }               \
        constexpr auto NS::u32::s8_saturate()              const noexcept -> strong::NS::s8  { return strong::NS::s8::saturate(raw); }             \
        constexpr auto NS::u32::s16_unchecked()            const noexcept -> strong::NS::s16 { return strong::NS::s16{ static_cast<rs16>(raw) }; } \
        constexpr auto NS::u32::s16_narrow()               const noexcept -> strong::NS::s16 { return strong::NS::s16::narrow(raw); }              \
        constexpr auto NS::u32::s16_saturate()             const noexcept -> strong::NS::s16 { return strong::NS::s16::saturate(raw); }            \
        constexpr auto NS::u32::s32_unchecked()            const noexcept -> strong::NS::s32 { return strong::NS::s32{ static_cast<rs32>(raw) }; } \
        constexpr auto NS::u32::s32_narrow()               const noexcept -> strong::NS::s32 { return strong::NS::s32::narrow(raw); }              \
        constexpr auto NS::u32::s32_saturate()             const noexcept -> strong::NS::s32 { return strong::NS::s32::saturate(raw); }
    #define RAWR_LIB_INTEGER_U64_DEFINITIONS(NS)\
        constexpr auto NS::u64::as_checked()               const noexcept -> checked    { return checked{ raw }; }                                 \
        constexpr auto NS::u64::as_wrapping()              const noexcept -> wrapping   { return wrapping{ raw }; }                                \
        constexpr auto NS::u64::as_saturating()            const noexcept -> saturating { return saturating{ raw }; }                              \
        constexpr auto NS::u64::u8_unchecked()             const noexcept -> strong::NS::u8  { return strong::NS::u8{ static_cast<ru8>(raw) }; }   \
        constexpr auto NS::u64::u8_narrow()                const noexcept -> strong::NS::u8  { return strong::NS::u8::narrow(raw); }               \
        constexpr auto NS::u64::u8_saturate()              const noexcept -> strong::NS::u8  { return strong::NS::u8::saturate(raw); }             \
        constexpr auto NS::u64::u16_unchecked()            const noexcept -> strong::NS::u16 { return strong::NS::u16{ static_cast<ru16>(raw) }; } \
        constexpr auto NS::u64::u16_narrow()               const noexcept -> strong::NS::u16 { return strong::NS::u16::narrow(raw); }              \
        constexpr auto NS::u64::u16_saturate()             const noexcept -> strong::NS::u16 { return strong::NS::u16::saturate(raw); }            \
        constexpr auto NS::u64::u32_unchecked()            const noexcept -> strong::NS::u32 { return strong::NS::u32{ static_cast<ru32>(raw) }; } \
        constexpr auto NS::u64::u32_narrow()               const noexcept -> strong::NS::u32 { return strong::NS::u32::narrow(raw); }              \
        constexpr auto NS::u64::u32_saturate()             const noexcept -> strong::NS::u32 { return strong::NS::u32::saturate(raw); }            \
        constexpr auto NS::u64::s8_unchecked()             const noexcept -> strong::NS::s8  { return strong::NS::s8{ static_cast<rs8>(raw) }; }   \
        constexpr auto NS::u64::s8_narrow()                const noexcept -> strong::NS::s8  { return strong::NS::s8::narrow(raw); }               \
        constexpr auto NS::u64::s8_saturate()              const noexcept -> strong::NS::s8  { return strong::NS::s8::saturate(raw); }             \
        constexpr auto NS::u64::s16_unchecked()            const noexcept -> strong::NS::s16 { return strong::NS::s16{ static_cast<rs16>(raw) }; } \
        constexpr auto NS::u64::s16_narrow()               const noexcept -> strong::NS::s16 { return strong::NS::s16::narrow(raw); }              \
        constexpr auto NS::u64::s16_saturate()             const noexcept -> strong::NS::s16 { return strong::NS::s16::saturate(raw); }            \
        constexpr auto NS::u64::s32_unchecked()            const noexcept -> strong::NS::s32 { return strong::NS::s32{ static_cast<rs32>(raw) }; } \
        constexpr auto NS::u64::s32_narrow()               const noexcept -> strong::NS::s32 { return strong::NS::s32::narrow(raw); }              \
        constexpr auto NS::u64::s32_saturate()             const noexcept -> strong::NS::s32 { return strong::NS::s32::saturate(raw); }            \
        constexpr auto NS::u64::s64_unchecked()            const noexcept -> strong::NS::s64 { return strong::NS::s64{ static_cast<rs64>(raw) }; } \
        constexpr auto NS::u64::s64_narrow()               const noexcept -> strong::NS::s64 { return strong::NS::s64::narrow(raw); }              \
        constexpr auto NS::u64::s64_saturate()             const noexcept -> strong::NS::s64 { return strong::NS::s64::saturate(raw); }
    #define RAWR_LIB_INTEGER_S8_DEFINITIONS(NS)\
        constexpr auto NS::s8::as_checked()                const noexcept -> checked    { return checked{ raw }; }                                 \
        constexpr auto NS::s8::as_wrapping()               const noexcept -> wrapping   { return wrapping{ raw }; }                                \
        constexpr auto NS::s8::as_saturating()             const noexcept -> saturating { return saturating{ raw }; }                              \
        constexpr auto NS::s8::s16()                       const noexcept -> strong::NS::s16 { return strong::NS::s16{ static_cast<rs16>(raw) }; } \
        constexpr auto NS::s8::s32()                       const noexcept -> strong::NS::s32 { return strong::NS::s32{ static_cast<rs32>(raw) }; } \
        constexpr auto NS::s8::s64()                       const noexcept -> strong::NS::s64 { return strong::NS::s64{ static_cast<rs64>(raw) }; } \
        constexpr      NS::s8::operator strong::NS::s16()  const noexcept { return this->s16(); }                                                  \
        constexpr      NS::s8::operator strong::NS::s32()  const noexcept { return this->s32(); }                                                  \
        constexpr      NS::s8::operator strong::NS::s64()  const noexcept { return this->s64(); }                                                  \
        constexpr auto NS::s8::u8_unchecked()              const noexcept -> strong::NS::u8  { return strong::NS::u8{ static_cast<ru8>(raw) }; }   \
        constexpr auto NS::s8::u8_narrow()                 const noexcept -> strong::NS::u8  { return strong::NS::u8::narrow(raw); }               \
        constexpr auto NS::s8::u8_saturate()               const noexcept -> strong::NS::u8  { return strong::NS::u8::saturate(raw); }             \
        constexpr auto NS::s8::u16_unchecked()             const noexcept -> strong::NS::u16 { return strong::NS::u16{ static_cast<ru16>(raw) }; } \
        constexpr auto NS::s8::u16_narrow()                const noexcept -> strong::NS::u16 { return strong::NS::u16::narrow(raw); }              \
        constexpr auto NS::s8::u16_saturate()              const noexcept -> strong::NS::u16 { return strong::NS::u16::saturate(raw); }            \
        constexpr auto NS::s8::u32_unchecked()             const noexcept -> strong::NS::u32 { return strong::NS::u32{ static_cast<ru32>(raw) }; } \
        constexpr auto NS::s8::u32_narrow()                const noexcept -> strong::NS::u32 { return strong::NS::u32::narrow(raw); }              \
        constexpr auto NS::s8::u32_saturate()              const noexcept -> strong::NS::u32 { return strong::NS::u32::saturate(raw); }            \
        constexpr auto NS::s8::u64_unchecked()             const noexcept -> strong::NS::u64 { return strong::NS::u64{ static_cast<ru64>(raw) }; } \
        constexpr auto NS::s8::u64_narrow()                const noexcept -> strong::NS::u64 { return strong::NS::u64::narrow(raw); }              \
        constexpr auto NS::s8::u64_saturate()              const noexcept -> strong::NS::u64 { return strong::NS::u64::saturate(raw); }
    #define RAWR_LIB_INTEGER_S16_DEFINITIONS(NS)\
        constexpr auto NS::s16::as_checked()               const noexcept -> checked    { return checked{ raw }; }                                 \
        constexpr auto NS::s16::as_wrapping()              const noexcept -> wrapping   { return wrapping{ raw }; }                                \
        constexpr auto NS::s16::as_saturating()            const noexcept -> saturating { return saturating{ raw }; }                              \
        constexpr auto NS::s16::s32()                      const noexcept -> strong::NS::s32 { return strong::NS::s32{ static_cast<rs32>(raw) }; } \
        constexpr auto NS::s16::s64()                      const noexcept -> strong::NS::s64 { return strong::NS::s64{ static_cast<rs64>(raw) }; } \
        constexpr      NS::s16::operator strong::NS::s32() const noexcept { return this->s32(); }                                                  \
        constexpr      NS::s16::operator strong::NS::s64() const noexcept { return this->s64(); }                                                  \
        constexpr auto NS::s16::u8_unchecked()             const noexcept -> strong::NS::u8  { return strong::NS::u8{ static_cast<ru8>(raw) }; }   \
        constexpr auto NS::s16::u8_narrow()                const noexcept -> strong::NS::u8  { return strong::NS::u8::narrow(raw); }               \
        constexpr auto NS::s16::u8_saturate()              const noexcept -> strong::NS::u8  { return strong::NS::u8::saturate(raw); }             \
        constexpr auto NS::s16::u16_unchecked()            const noexcept -> strong::NS::u16 { return strong::NS::u16{ static_cast<ru16>(raw) }; } \
        constexpr auto NS::s16::u16_narrow()               const noexcept -> strong::NS::u16 { return strong::NS::u16::narrow(raw); }              \
        constexpr auto NS::s16::u16_saturate()             const noexcept -> strong::NS::u16 { return strong::NS::u16::saturate(raw); }            \
        constexpr auto NS::s16::u32_unchecked()            const noexcept -> strong::NS::u32 { return strong::NS::u32{ static_cast<ru32>(raw) }; } \
        constexpr auto NS::s16::u32_narrow()               const noexcept -> strong::NS::u32 { return strong::NS::u32::narrow(raw); }              \
        constexpr auto NS::s16::u32_saturate()             const noexcept -> strong::NS::u32 { return strong::NS::u32::saturate(raw); }            \
        constexpr auto NS::s16::u64_unchecked()            const noexcept -> strong::NS::u64 { return strong::NS::u64{ static_cast<ru64>(raw) }; } \
        constexpr auto NS::s16::u64_narrow()               const noexcept -> strong::NS::u64 { return strong::NS::u64::narrow(raw); }              \
        constexpr auto NS::s16::u64_saturate()             const noexcept -> strong::NS::u64 { return strong::NS::u64::saturate(raw); }            \
        constexpr auto NS::s16::s8_unchecked()             const noexcept -> strong::NS::s8  { return strong::NS::s8{ static_cast<rs8>(raw) }; }   \
        constexpr auto NS::s16::s8_narrow()                const noexcept -> strong::NS::s8  { return strong::NS::s8::narrow(raw); }               \
        constexpr auto NS::s16::s8_saturate()              const noexcept -> strong::NS::s8  { return strong::NS::s8::saturate(raw); }
    #define RAWR_LIB_INTEGER_S32_DEFINITIONS(NS)\
        constexpr auto NS::s32::as_checked()               const noexcept -> checked    { return checked{ raw }; }                                 \
        constexpr auto NS::s32::as_wrapping()              const noexcept -> wrapping   { return wrapping{ raw }; }                                \
        constexpr auto NS::s32::as_saturating()            const noexcept -> saturating { return saturating{ raw }; }                              \
        constexpr auto NS::s32::s64()                      const noexcept -> strong::NS::s64 { return strong::NS::s64{ static_cast<rs64>(raw) }; } \
        constexpr      NS::s32::operator strong::NS::s64() const noexcept { return this->s64(); }                                                  \
        constexpr auto NS::s32::u8_unchecked()             const noexcept -> strong::NS::u8  { return strong::NS::u8{ static_cast<ru8>(raw) }; }   \
        constexpr auto NS::s32::u8_narrow()                const noexcept -> strong::NS::u8  { return strong::NS::u8::narrow(raw); }               \
        constexpr auto NS::s32::u8_saturate()              const noexcept -> strong::NS::u8  { return strong::NS::u8::saturate(raw); }             \
        constexpr auto NS::s32::u16_unchecked()            const noexcept -> strong::NS::u16 { return strong::NS::u16{ static_cast<ru16>(raw) }; } \
        constexpr auto NS::s32::u16_narrow()               const noexcept -> strong::NS::u16 { return strong::NS::u16::narrow(raw); }              \
        constexpr auto NS::s32::u16_saturate()             const noexcept -> strong::NS::u16 { return strong::NS::u16::saturate(raw); }            \
        constexpr auto NS::s32::u32_unchecked()            const noexcept -> strong::NS::u32 { return strong::NS::u32{ static_cast<ru32>(raw) }; } \
        constexpr auto NS::s32::u32_narrow()               const noexcept -> strong::NS::u32 { return strong::NS::u32::narrow(raw); }              \
        constexpr auto NS::s32::u32_saturate()             const noexcept -> strong::NS::u32 { return strong::NS::u32::saturate(raw); }            \
        constexpr auto NS::s32::u64_unchecked()            const noexcept -> strong::NS::u64 { return strong::NS::u64{ static_cast<ru64>(raw) }; } \
        constexpr auto NS::s32::u64_narrow()               const noexcept -> strong::NS::u64 { return strong::NS::u64::narrow(raw); }              \
        constexpr auto NS::s32::u64_saturate()             const noexcept -> strong::NS::u64 { return strong::NS::u64::saturate(raw); }            \
        constexpr auto NS::s32::s8_unchecked()             const noexcept -> strong::NS::s8  { return strong::NS::s8{ static_cast<rs8>(raw) }; }   \
        constexpr auto NS::s32::s8_narrow()                const noexcept -> strong::NS::s8  { return strong::NS::s8::narrow(raw); }               \
        constexpr auto NS::s32::s8_saturate()              const noexcept -> strong::NS::s8  { return strong::NS::s8::saturate(raw); }             \
        constexpr auto NS::s32::s16_unchecked()            const noexcept -> strong::NS::s16 { return strong::NS::s16{ static_cast<rs16>(raw) }; } \
        constexpr auto NS::s32::s16_narrow()               const noexcept -> strong::NS::s16 { return strong::NS::s16::narrow(raw); }              \
        constexpr auto NS::s32::s16_saturate()             const noexcept -> strong::NS::s16 { return strong::NS::s16::saturate(raw); }
    #define RAWR_LIB_INTEGER_S64_DEFINITIONS(NS)\
        constexpr auto NS::s64::as_checked()               const noexcept -> checked    { return checked{ raw }; }                                 \
        constexpr auto NS::s64::as_wrapping()              const noexcept -> wrapping   { return wrapping{ raw }; }                                \
        constexpr auto NS::s64::as_saturating()            const noexcept -> saturating { return saturating{ raw }; }                              \
        constexpr auto NS::s64::u8_unchecked()             const noexcept -> strong::NS::u8  { return strong::NS::u8{ static_cast<ru8>(raw) }; }   \
        constexpr auto NS::s64::u8_narrow()                const noexcept -> strong::NS::u8  { return strong::NS::u8::narrow(raw); }               \
        constexpr auto NS::s64::u8_saturate()              const noexcept -> strong::NS::u8  { return strong::NS::u8::saturate(raw); }             \
        constexpr auto NS::s64::u16_unchecked()            const noexcept -> strong::NS::u16 { return strong::NS::u16{ static_cast<ru16>(raw) }; } \
        constexpr auto NS::s64::u16_narrow()               const noexcept -> strong::NS::u16 { return strong::NS::u16::narrow(raw); }              \
        constexpr auto NS::s64::u16_saturate()             const noexcept -> strong::NS::u16 { return strong::NS::u16::saturate(raw); }            \
        constexpr auto NS::s64::u32_unchecked()            const noexcept -> strong::NS::u32 { return strong::NS::u32{ static_cast<ru32>(raw) }; } \
        constexpr auto NS::s64::u32_narrow()               const noexcept -> strong::NS::u32 { return strong::NS::u32::narrow(raw); }              \
        constexpr auto NS::s64::u32_saturate()             const noexcept -> strong::NS::u32 { return strong::NS::u32::saturate(raw); }            \
        constexpr auto NS::s64::u64_unchecked()            const noexcept -> strong::NS::u64 { return strong::NS::u64{ static_cast<ru64>(raw) }; } \
        constexpr auto NS::s64::u64_narrow()               const noexcept -> strong::NS::u64 { return strong::NS::u64::narrow(raw); }              \
        constexpr auto NS::s64::u64_saturate()             const noexcept -> strong::NS::u64 { return strong::NS::u64::saturate(raw); }            \
        constexpr auto NS::s64::s8_unchecked()             const noexcept -> strong::NS::s8  { return strong::NS::s8{ static_cast<rs8>(raw) }; }   \
        constexpr auto NS::s64::s8_narrow()                const noexcept -> strong::NS::s8  { return strong::NS::s8::narrow(raw); }               \
        constexpr auto NS::s64::s8_saturate()              const noexcept -> strong::NS::s8  { return strong::NS::s8::saturate(raw); }             \
        constexpr auto NS::s64::s16_unchecked()            const noexcept -> strong::NS::s16 { return strong::NS::s16{ static_cast<rs16>(raw) }; } \
        constexpr auto NS::s64::s16_narrow()               const noexcept -> strong::NS::s16 { return strong::NS::s16::narrow(raw); }              \
        constexpr auto NS::s64::s16_saturate()             const noexcept -> strong::NS::s16 { return strong::NS::s16::saturate(raw); }            \
        constexpr auto NS::s64::s32_unchecked()            const noexcept -> strong::NS::s32 { return strong::NS::s32{ static_cast<rs32>(raw) }; } \
        constexpr auto NS::s64::s32_narrow()               const noexcept -> strong::NS::s32 { return strong::NS::s32::narrow(raw); }              \
        constexpr auto NS::s64::s32_saturate()             const noexcept -> strong::NS::s32 { return strong::NS::s32::saturate(raw); }

    RAWR_LIB_INTEGER_DEFINE3(RAWR_LIB_INTEGER_U8_DEFINITIONS)
    RAWR_LIB_INTEGER_DEFINE3(RAWR_LIB_INTEGER_U16_DEFINITIONS)
    RAWR_LIB_INTEGER_DEFINE3(RAWR_LIB_INTEGER_U32_DEFINITIONS)
    RAWR_LIB_INTEGER_DEFINE3(RAWR_LIB_INTEGER_U64_DEFINITIONS)
    RAWR_LIB_INTEGER_DEFINE3(RAWR_LIB_INTEGER_S8_DEFINITIONS)
    RAWR_LIB_INTEGER_DEFINE3(RAWR_LIB_INTEGER_S16_DEFINITIONS)
    RAWR_LIB_INTEGER_DEFINE3(RAWR_LIB_INTEGER_S32_DEFINITIONS)
    RAWR_LIB_INTEGER_DEFINE3(RAWR_LIB_INTEGER_S64_DEFINITIONS)

    #undef RAWR_LIB_INTEGER_U8_DEFINITIONS
    #undef RAWR_LIB_INTEGER_U16_DEFINITIONS
    #undef RAWR_LIB_INTEGER_U32_DEFINITIONS
    #undef RAWR_LIB_INTEGER_U64_DEFINITIONS
    #undef RAWR_LIB_INTEGER_S8_DEFINITIONS
    #undef RAWR_LIB_INTEGER_S16_DEFINITIONS
    #undef RAWR_LIB_INTEGER_S32_DEFINITIONS
    #undef RAWR_LIB_INTEGER_S64_DEFINITIONS
    #undef RAWR_LIB_INTEGER_DEFINE3

    /// Then we alias the default-behaviour back into the strong:: namespace.

    using checked::u8; using checked::u16; using checked::u32; using checked::u64;
    using checked::s8; using checked::s16; using checked::s32; using checked::s64;

    /// And finally define the literals.

    inline namespace literals
    {
        consteval auto operator""_u8(unsigned long long val)       noexcept { return u8(val);  }
        consteval auto operator""_u16(unsigned long long val)      noexcept { return u16(val); }
        consteval auto operator""_u32(unsigned long long val)      noexcept { return u32(val); }
        consteval auto operator""_u64(unsigned long long val)      noexcept { return u64(val); }
        consteval auto operator""_s8(unsigned long long val)       noexcept { return s8(val);  }
        consteval auto operator""_s16(unsigned long long val)      noexcept { return s16(val); }
        consteval auto operator""_s32(unsigned long long val)      noexcept { return s32(val); }
        consteval auto operator""_s64(unsigned long long val)      noexcept { return s64(val); }
        consteval auto operator""_u8_sat(unsigned long long val)   noexcept { return u8::saturating(val);  }
        consteval auto operator""_u16_sat(unsigned long long val)  noexcept { return u16::saturating(val); }
        consteval auto operator""_u32_sat(unsigned long long val)  noexcept { return u32::saturating(val); }
        consteval auto operator""_u64_sat(unsigned long long val)  noexcept { return u64::saturating(val); }
        consteval auto operator""_s8_sat(unsigned long long val)   noexcept { return s8::saturating(val);  }
        consteval auto operator""_s16_sat(unsigned long long val)  noexcept { return s16::saturating(val); }
        consteval auto operator""_s32_sat(unsigned long long val)  noexcept { return s32::saturating(val); }
        consteval auto operator""_s64_sat(unsigned long long val)  noexcept { return s64::saturating(val); }
        consteval auto operator""_u8_wrap(unsigned long long val)  noexcept { return u8::wrapping(val);  }
        consteval auto operator""_u16_wrap(unsigned long long val) noexcept { return u16::wrapping(val); }
        consteval auto operator""_u32_wrap(unsigned long long val) noexcept { return u32::wrapping(val); }
        consteval auto operator""_u64_wrap(unsigned long long val) noexcept { return u64::wrapping(val); }
        consteval auto operator""_s8_wrap(unsigned long long val)  noexcept { return s8::wrapping(val);  }
        consteval auto operator""_s16_wrap(unsigned long long val) noexcept { return s16::wrapping(val); }
        consteval auto operator""_s32_wrap(unsigned long long val) noexcept { return s32::wrapping(val); }
        consteval auto operator""_s64_wrap(unsigned long long val) noexcept { return s64::wrapping(val); }
        consteval auto operator""_u8_chk(unsigned long long val)   noexcept { return u8::checked(val);  }
        consteval auto operator""_u16_chk(unsigned long long val)  noexcept { return u16::checked(val); }
        consteval auto operator""_u32_chk(unsigned long long val)  noexcept { return u32::checked(val); }
        consteval auto operator""_u64_chk(unsigned long long val)  noexcept { return u64::checked(val); }
        consteval auto operator""_s8_chk(unsigned long long val)   noexcept { return s8::checked(val);  }
        consteval auto operator""_s16_chk(unsigned long long val)  noexcept { return s16::checked(val); }
        consteval auto operator""_s32_chk(unsigned long long val)  noexcept { return s32::checked(val); }
        consteval auto operator""_s64_chk(unsigned long long val)  noexcept { return s64::checked(val); }
    }
}

/// Oh also i guess we need to specialize these traits too:
#define RAWR_LIB_INTEGER_SPECIALIZE3(Type)                                                                                                                \
    template <> struct rawr::trait::uint<rawr::strong::saturating::Type> { static constexpr auto value = rawr::strong::saturating::Type::is_unsigned;  }; \
    template <> struct rawr::trait::sint<rawr::strong::saturating::Type> { static constexpr auto value = rawr::strong::saturating::Type::is_signed;    }; \
    template <> struct rawr::trait::uint<rawr::strong::wrapping::Type>   { static constexpr auto value = rawr::strong::wrapping::Type::is_unsigned;  };   \
    template <> struct rawr::trait::sint<rawr::strong::wrapping::Type>   { static constexpr auto value = rawr::strong::wrapping::Type::is_signed;    };   \
    template <> struct rawr::trait::uint<rawr::strong::checked::Type>    { static constexpr auto value = rawr::strong::checked::Type::is_unsigned;  };    \
    template <> struct rawr::trait::sint<rawr::strong::checked::Type>    { static constexpr auto value = rawr::strong::checked::Type::is_signed;    };

RAWR_LIB_INTEGER_SPECIALIZE3(u8)
RAWR_LIB_INTEGER_SPECIALIZE3(u16)
RAWR_LIB_INTEGER_SPECIALIZE3(u32)
RAWR_LIB_INTEGER_SPECIALIZE3(u64)
RAWR_LIB_INTEGER_SPECIALIZE3(s8)
RAWR_LIB_INTEGER_SPECIALIZE3(s16)
RAWR_LIB_INTEGER_SPECIALIZE3(s32)
RAWR_LIB_INTEGER_SPECIALIZE3(s64)

#undef RAWR_LIB_INTEGER_SPECIALIZE3
