//// rawr/lib/integer/base.hpp
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    import rawr.lib.bits;
#else
    #include "rawr/lib/bits.hpp"
#endif
#include "rawr/lib/detection.pp"

namespace rawr::inline lib::inline integer::inline base::detail
{
    // Prefer aint_[max|min] to using these directly, those dont need a call (are constexpr variables),
    // are type safe(r) via the aint concept and
    constexpr auto uint_max(bitwidth const bits) noexcept -> unsigned long long { return bits == biw64 ? ~0ULL : (1ULL << bits.val) - 1ULL; }
    constexpr auto sint_max(bitwidth const bits) noexcept -> long long          { return static_cast<long long>(uint_max(bits) >> 1U); }
    constexpr auto sint_min(bitwidth const bits) noexcept -> long long          { return ~sint_max(bits); } // two's complement bitwise NOT, defined in C++20.

    // Same as above for ruint_capable, prefer rawr::ruint_capable<>.
    // Correctly handles types with overlapping sizes. Choosing the first match.
    template <unsigned long long Num>
    consteval auto ruint_capable()
    {
             if constexpr(Num > uint_max(bitsof<unsigned long>))  { return Num; }
        else if constexpr(Num > uint_max(bitsof<unsigned int>))   { return static_cast<unsigned long>(Num); }
        else if constexpr(Num > uint_max(bitsof<unsigned short>)) { return static_cast<unsigned int>(Num); }
        else if constexpr(Num > uint_max(bitsof<unsigned char>))  { return static_cast<unsigned short>(Num); }
        else                                                      { return static_cast<unsigned char>(Num); }
    }

    // NOTE: These could just as well belong on a public library layer
    //       somewhere else instead of integer::detail.
    template <bitwidth Bits, typename... Types>
    struct select_type_by_size { static_assert(Bits == biw0, "No type of this size"); };

    template <bitwidth Bits, typename Type, typename... Rest>
    requires (bitsof<Type> == Bits)
    struct select_type_by_size<Bits, Type, Rest...> { using type = Type; };

    template <bitwidth Bits, typename Type, typename... Rest>
    struct select_type_by_size<Bits, Type, Rest...> : select_type_by_size<Bits, Rest...> {};

    // Needed by the int_from_literal function. Do not define these.
    #if RAWR_COMPILER_FAMILY_GNU
        [[gnu::error("Literal underflows target type")]]      void lit_underflows_target_min() noexcept;
        [[gnu::error("Literal overflows target type")]]       void lit_overflows_target_max()  noexcept;
        [[gnu::error("Negative literal to unsigned target")]] void lit_negative_to_unsigned()  noexcept;
    #else
        void lit_underflows_target_min() noexcept;
        void lit_overflows_target_max() noexcept;
        void lit_negative_to_unsigned() noexcept;
    #endif

    // MSVC is quite picky with __is_same.
    #if RAWR_COMPILER_MSVC
        template <typename T, typename U> struct is_same       { static constexpr auto value = false; };
        template <typename T>             struct is_same<T, T> { static constexpr auto value = true; };
    #endif
}

RAWR_EXPORT namespace rawr::inline lib::inline integer::inline base
{
    // These encode only RAW integer types.
    // NOTE: We use raw intrinsics since intrin:: actually depends on integer.base.
    // NOTE: raint32, raint64, rsint32, etc, still make some sort of sense since it could feasibly be referring
    //       to two (or more) different types.
    //       For example, for rsint: int and long sometimes are the same size on some architectures.
    #if RAWR_COMPILER_MSVC
        template <typename T, bitwidth Bits = biw0> concept rsint = (detail::is_same<T, char>::value || detail::is_same<T, signed   char>::value || detail::is_same<T, signed   short>::value || detail::is_same<T, signed   int>::value || detail::is_same<T, signed   long>::value || detail::is_same<T, signed   long long>::value) && (Bits == biw0 || bitsof<T> == Bits);
        template <typename T, bitwidth Bits = biw0> concept ruint =                                    (detail::is_same<T, unsigned char>::value || detail::is_same<T, unsigned short>::value || detail::is_same<T, unsigned int>::value || detail::is_same<T, unsigned long>::value || detail::is_same<T, unsigned long long>::value) && (Bits == biw0 || bitsof<T> == Bits);
    #else
        template <typename T, bitwidth Bits = biw0> concept rsint = (__is_same(T, char)              || __is_same(T, signed   char)              || __is_same(T, signed   short)              || __is_same(T, signed   int)              || __is_same(T, signed   long)              || __is_same(T, signed long long))    && (Bits == biw0 || bitsof<T> == Bits);
        template <typename T, bitwidth Bits = biw0> concept ruint =                                    (__is_same(T, unsigned char)              || __is_same(T, unsigned short)              || __is_same(T, unsigned int)              || __is_same(T, unsigned long)              || __is_same(T, unsigned long long))  && (Bits == biw0 || bitsof<T> == Bits);
    #endif
    template <typename T> concept ruint8  = ruint<T, biw8>;
    template <typename T> concept ruint16 = ruint<T, biw16>;
    template <typename T> concept ruint32 = ruint<T, biw32>;
    template <typename T> concept ruint64 = ruint<T, biw64>;
    template <typename T> concept rsint8  = rsint<T, biw8>;
    template <typename T> concept rsint16 = rsint<T, biw16>;
    template <typename T> concept rsint32 = rsint<T, biw32>;
    template <typename T> concept rsint64 = rsint<T, biw64>;
    template <typename T, bitwidth Bits = biw0> concept raint = rsint<T, Bits> || ruint<T, Bits>;
    template <typename T> concept raint8  = raint<T, biw8>;
    template <typename T> concept raint16 = raint<T, biw16>;
    template <typename T> concept raint32 = raint<T, biw32>;
    template <typename T> concept raint64 = raint<T, biw64>;

    // uint and sint are opt-in. Specialize as needed.
    namespace trait
    {
        template <typename T> struct uint { static constexpr auto value = false; };
        template <typename T> struct sint { static constexpr auto value = false; };
    }
    // Such as (specializing for the raw integer types):
    template <ruint T> struct trait::uint<T> { static constexpr auto value = true; };
    template <rsint T> struct trait::sint<T> { static constexpr auto value = true; };

    // These encode any integer type, raw or custom.
    template <typename T, bitwidth Bits = biw0> concept uint = trait::uint<T>::value && (Bits == biw0 || bitsof<T> == Bits);
    template <typename T> concept uint8  = uint<T, biw8>;
    template <typename T> concept uint16 = uint<T, biw16>;
    template <typename T> concept uint32 = uint<T, biw32>;
    template <typename T> concept uint64 = uint<T, biw64>;
    template <typename T, bitwidth Bits = biw0> concept sint = trait::sint<T>::value && (Bits == biw0 || bitsof<T> == Bits);
    template <typename T> concept sint8  = sint<T, biw8>;
    template <typename T> concept sint16 = sint<T, biw16>;
    template <typename T> concept sint32 = sint<T, biw32>;
    template <typename T> concept sint64 = sint<T, biw64>;
    template <typename T, bitwidth Bits = biw0> concept aint = uint<T, Bits> || sint<T, Bits>;
    template <typename T> concept aint8  = aint<T, biw8>;
    template <typename T> concept aint16 = aint<T, biw16>;
    template <typename T> concept aint32 = aint<T, biw32>;
    template <typename T> concept aint64 = aint<T, biw64>;

    // For completeness, heres how you detect ONLY custom integer types.
    template <typename T, bitwidth Bits = biw0> concept cuint = (!ruint<T> && trait::uint<T>::value) && (Bits == biw0 || bitsof<T> == Bits);
    template <typename T> concept cuint8  = cuint<T, biw8>;
    template <typename T> concept cuint16 = cuint<T, biw16>;
    template <typename T> concept cuint32 = cuint<T, biw32>;
    template <typename T> concept cuint64 = cuint<T, biw64>;
    template <typename T, bitwidth Bits = biw0> concept csint = (!rsint<T> && trait::sint<T>::value) && (Bits == biw0 || bitsof<T> == Bits);
    template <typename T> concept csint8  = csint<T, biw8>;
    template <typename T> concept csint16 = csint<T, biw16>;
    template <typename T> concept csint32 = csint<T, biw32>;
    template <typename T> concept csint64 = csint<T, biw64>;
    template <typename T, bitwidth Bits = biw0> concept caint = cuint<T, Bits> || csint<T, Bits>;
    template <typename T> concept caint8  = caint<T, biw8>;
    template <typename T> concept caint16 = caint<T, biw16>;
    template <typename T> concept caint32 = caint<T, biw32>;
    template <typename T> concept caint64 = caint<T, biw64>;

    template <aint T>
    constexpr T aint_max = sint<T>
        ? static_cast<T>(detail::sint_max(bitsof<T>))
        : static_cast<T>(detail::uint_max(bitsof<T>));
    template <aint T>
    constexpr T aint_min = sint<T>
        ? static_cast<T>(detail::sint_min(bitsof<T>))
        : T{0};

    template <bitwidth Bits>     using ruint_exact   = detail::select_type_by_size<Bits, unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>::type;
    template <bitwidth Bits>     using rsint_exact   = detail::select_type_by_size<Bits,   signed char,   signed short,   signed int,   signed long,   signed long long>::type;
    template <unsigned long Num> using ruint_capable = decltype(detail::ruint_capable<Num>());
}

namespace rawr::inline lib::inline integer::inline base::detail
{
    template <aint T> struct raint_of_t;
    template <sint T> struct raint_of_t<T> { using type = rsint_exact<bitsof<T>>; };
    template <uint T> struct raint_of_t<T> { using type = ruint_exact<bitsof<T>>; };
}

RAWR_EXPORT namespace rawr::inline lib::inline integer::inline base
{
    // Gets the corresponding RAW integer type for a given T.
    template <aint T> using ruint_of = ruint_exact<bitsof<T>>;
    template <aint T> using rsint_of = rsint_exact<bitsof<T>>;
    // Automatically gets the correspoding ru* or rs* for a given aint of the same size.
    // Behaves like a std::conditional_t<sint<T>, rsint_exact<sizeof(T)>, ruint_exact<sizeof(T)>.
    template <aint T> using raint_of = detail::raint_of_t<T>::type;

    // Safely construct an integer of a Target type from an arbitrary literal.
    template <aint Target>
    consteval auto aint_from_literal(auto val) noexcept -> Target
    {
        constexpr bool v_signed = sint<decltype(val)>;
        constexpr bool t_signed = sint<Target>;

        if constexpr (v_signed == t_signed) {
            if (val < aint_min<Target>) { detail::lit_underflows_target_min(); }
            if (val > aint_max<Target>) { detail::lit_overflows_target_max(); }
        }
        else if constexpr (v_signed && !t_signed) {
            if (val < 0)                                                                                  { detail::lit_negative_to_unsigned(); }
            if (static_cast<unsigned long long>(val) < static_cast<unsigned long long>(aint_min<Target>)) { detail::lit_underflows_target_min(); }
            if (static_cast<unsigned long long>(val) > static_cast<unsigned long long>(aint_max<Target>)) { detail::lit_overflows_target_max(); }
        }
        else {
            if constexpr (aint_min<Target> >= 0) {
                if (static_cast<unsigned long long>(val) < static_cast<unsigned long long>(aint_min<Target>))
                { detail::lit_underflows_target_min(); }
            }
            if constexpr (aint_max<Target> < 0) { detail::lit_overflows_target_max();
            } else {
                if (static_cast<unsigned long long>(val) > static_cast<unsigned long long>(aint_max<Target>))
                { detail::lit_overflows_target_max(); }
            }
        }

        return static_cast<Target>(val);
    }

    template <aint Target>
    constexpr auto aint_saturating_cast(auto val) noexcept -> Target
    {
        using V = decltype(val);
        constexpr auto v_signed = sint<decltype(val)>;
        constexpr auto t_signed = sint<Target>;

        if constexpr (v_signed && !t_signed) {
            if (val < V{0}) { return aint_min<Target>; }
            if constexpr (sizeof(V) > sizeof(Target)) {
                if (val > static_cast<V>(aint_max<Target>)) { return aint_max<Target>; }
            }
        } else if constexpr (!v_signed && t_signed) {
            if constexpr (sizeof(V) >= sizeof(Target)) {
                if (val > static_cast<V>(aint_max<Target>)) { return aint_max<Target>; }
            }
        } else {
            if (val > static_cast<V>(aint_max<Target>)) { return aint_max<Target>; }
            if (val < static_cast<V>(aint_min<Target>)) { return aint_min<Target>; }
        }
        return static_cast<Target>(val);
    }
}
