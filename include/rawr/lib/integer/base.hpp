#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/integer/base.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.lib.integer.base;
    import rawr.lib.bits;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/bits.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"

namespace rawr::inline lib::inline integer::inline base::detail
{
    // Prefer aint_[max|min] to using these directly, those dont need a call (are constexpr variables),
    // are type safe(r) via the Aint concept and
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

    // This specific formulation of a type selector works on GCC <= 13, the previous one would crash the compiler.
    // Likely due to bodged requires() implementation.
    template <bitwidth Bits, typename Type, typename... Rest>
    constexpr auto select_type_by_size_helper()
    {
             if constexpr (bitsof<Type> == Bits) { return static_cast<Type>(0);}
        else if constexpr (sizeof...(Rest) > 0)  { return select_type_by_size_helper<Bits, Rest...>();}
        else {
            static_assert(Bits == biw0, "No type of this size");
            return char{0};
        }
    }
    template <bitwidth Bits, typename... Types>
    using select_type_by_size = decltype(select_type_by_size_helper<Bits, Types...>());

    template <bitwidth Bits>
    struct rsint_exact
    {
        using type = typename detail::select_type_by_size<Bits,
            signed char,
            signed short,
            signed int,
            signed long,
            signed long long
            #if RAWR_HAS_INT128
                , __int128
            #endif
        >;
    };
    template <bitwidth Bits>
    struct ruint_exact
    {
        using type = typename detail::select_type_by_size<Bits,
            unsigned char,
            unsigned short,
            unsigned int,
            unsigned long,
            unsigned long long
            #if RAWR_HAS_INT128
                , unsigned __int128
            #endif
        >;
    };
    template <bitwidth Bits>
    struct rfloat_exact
    {
        using type = typename detail::select_type_by_size<Bits,
            float,
            double,
            long double
        >;
    };

    // Needed by the int_from_literal function. Do not define these.
	#if RAWR_COMPILER_GCC || (RAWR_COMPILER_CLANG && RAWR_COMPILER_VERSION_MAJOR >= 14)
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
    //       For example, for RSint: int and long sometimes are the same size on some architectures.
    #if RAWR_COMPILER_MSVC
        template <typename T, bitwidth Bits = biw0> concept RSint = (detail::is_same<T, signed   char>::value || detail::is_same<T, signed   short>::value || detail::is_same<T, signed   int>::value || detail::is_same<T, signed   long>::value || detail::is_same<T, signed   long long>::value) && (Bits == biw0 || bitsof<T> == Bits);
        template <typename T, bitwidth Bits = biw0> concept RUint = (detail::is_same<T, unsigned char>::value || detail::is_same<T, unsigned short>::value || detail::is_same<T, unsigned int>::value || detail::is_same<T, unsigned long>::value || detail::is_same<T, unsigned long long>::value) && (Bits == biw0 || bitsof<T> == Bits);
    #else
        template <typename T, bitwidth Bits = biw0> concept RSint = (
            __is_same(T, signed   char) || __is_same(T, signed   short) || __is_same(T, signed   int) || __is_same(T, signed   long) || __is_same(T, signed long long)
            #if RAWR_HAS_INT128
                || __is_same(T, __int128)
            #endif
            ) && (Bits == biw0 || bitsof<T> == Bits);
        template <typename T, bitwidth Bits = biw0> concept RUint = (
            __is_same(T, unsigned char) || __is_same(T, unsigned short) || __is_same(T, unsigned int) || __is_same(T, unsigned long) || __is_same(T, unsigned long long)
            #if RAWR_HAS_INT128
                || __is_same(T, unsigned __int128)
            #endif
            )  && (Bits == biw0 || bitsof<T> == Bits);
    #endif
    template <typename T> concept RUint8   = RUint<T, biw8>;
    template <typename T> concept RUint16  = RUint<T, biw16>;
    template <typename T> concept RUint32  = RUint<T, biw32>;
    template <typename T> concept RUint64  = RUint<T, biw64>;
    template <typename T> concept RUint128 = RUint<T, biw128>;
    template <typename T> concept RSint8   = RSint<T, biw8>;
    template <typename T> concept RSint16  = RSint<T, biw16>;
    template <typename T> concept RSint32  = RSint<T, biw32>;
    template <typename T> concept RSint64  = RSint<T, biw64>;
    template <typename T> concept RSint128 = RSint<T, biw128>;
    template <typename T, bitwidth Bits = biw0> concept RAint = RSint<T, Bits> || RUint<T, Bits>;
    template <typename T> concept RAint8   = RAint<T, biw8>;
    template <typename T> concept RAint16  = RAint<T, biw16>;
    template <typename T> concept RAint32  = RAint<T, biw32>;
    template <typename T> concept RAint64  = RAint<T, biw64>;
    template <typename T> concept RAint128 = RAint<T, biw128>;

    // Uint and Sint are opt-in. Specialize as needed.
    namespace trait
    {
        template <typename T> struct uint { static constexpr auto value = false; };
        template <typename T> struct sint { static constexpr auto value = false; };
    }
    // Such as (specializing for the raw integer types):
    template <RUint T> struct trait::uint<T> { static constexpr auto value = true; };
    template <RSint T> struct trait::sint<T> { static constexpr auto value = true; };

    // These encode any integer type, raw or custom.
    template <typename T, bitwidth Bits = biw0> concept Uint = trait::uint<T>::value && (Bits == biw0 || bitsof<T> == Bits);
    template <typename T> concept Uint8   = Uint<T, biw8>;
    template <typename T> concept Uint16  = Uint<T, biw16>;
    template <typename T> concept Uint32  = Uint<T, biw32>;
    template <typename T> concept Uint64  = Uint<T, biw64>;
    template <typename T> concept Uint128 = Uint<T, biw128>;
    template <typename T, bitwidth Bits = biw0> concept Sint = trait::sint<T>::value && (Bits == biw0 || bitsof<T> == Bits);
    template <typename T> concept Sint8   = Sint<T, biw8>;
    template <typename T> concept Sint16  = Sint<T, biw16>;
    template <typename T> concept Sint32  = Sint<T, biw32>;
    template <typename T> concept Sint64  = Sint<T, biw64>;
    template <typename T> concept Sint128 = Sint<T, biw128>;
    template <typename T, bitwidth Bits = biw0> concept Aint = Uint<T, Bits> || Sint<T, Bits>;
    template <typename T> concept Aint8   = Aint<T, biw8>;
    template <typename T> concept Aint16  = Aint<T, biw16>;
    template <typename T> concept Aint32  = Aint<T, biw32>;
    template <typename T> concept Aint64  = Aint<T, biw64>;
    template <typename T> concept Aint128 = Aint<T, biw128>;

    // For completeness, heres how you detect ONLY custom integer types.
    template <typename T, bitwidth Bits = biw0> concept CUint = (!RUint<T> && trait::uint<T>::value) && (Bits == biw0 || bitsof<T> == Bits);
    template <typename T> concept CUint8   = CUint<T, biw8>;
    template <typename T> concept CUint16  = CUint<T, biw16>;
    template <typename T> concept CUint32  = CUint<T, biw32>;
    template <typename T> concept CUint64  = CUint<T, biw64>;
    template <typename T> concept CUint128 = CUint<T, biw128>;
    template <typename T, bitwidth Bits = biw0> concept CSint = (!RSint<T> && trait::sint<T>::value) && (Bits == biw0 || bitsof<T> == Bits);
    template <typename T> concept CSint8   = CSint<T, biw8>;
    template <typename T> concept CSint16  = CSint<T, biw16>;
    template <typename T> concept CSint32  = CSint<T, biw32>;
    template <typename T> concept CSint64  = CSint<T, biw64>;
    template <typename T> concept CSint128 = CSint<T, biw128>;
    template <typename T, bitwidth Bits = biw0> concept CAint = CUint<T, Bits> || CSint<T, Bits>;
    template <typename T> concept CAint8   = CAint<T, biw8>;
    template <typename T> concept CAint16  = CAint<T, biw16>;
    template <typename T> concept CAint32  = CAint<T, biw32>;
    template <typename T> concept CAint64  = CAint<T, biw64>;
    template <typename T> concept CAint128 = CAint<T, biw128>;

    template <Aint T>
    constexpr T aint_max = Sint<T>
        ? static_cast<T>(detail::sint_max(bitsof<T>))
        : static_cast<T>(detail::uint_max(bitsof<T>));
    template <Aint T>
    constexpr T aint_min = Sint<T>
        ? static_cast<T>(detail::sint_min(bitsof<T>))
        : T{0};

	template <bitwidth Bits>     using rsint_exact   = typename detail::rsint_exact<Bits>::type;
	template <bitwidth Bits>     using ruint_exact   = typename detail::ruint_exact<Bits>::type;
	template <bitwidth Bits>     using rfloat_exact  = typename detail::rfloat_exact<Bits>::type;
	template <unsigned long Num> using ruint_capable = decltype(detail::ruint_capable<Num>());
}

namespace rawr::inline lib::inline integer::inline base::detail
{
    template <Aint T> struct raint_of_t;
    template <Sint T> struct raint_of_t<T> { using type = base::rsint_exact<bitsof<T>>; };
    template <Uint T> struct raint_of_t<T> { using type = base::ruint_exact<bitsof<T>>; };
}

RAWR_EXPORT namespace rawr::inline lib::inline integer::inline base
{
    // Gets the corresponding RAW integer type for a given T.
    template <Aint T> using ruint_of = ruint_exact<bitsof<T>>;
    template <Aint T> using rsint_of = rsint_exact<bitsof<T>>;
    // Automatically gets the correspoding ru* or rs* for a given Aint of the same size.
    // Behaves like a std::conditional_t<Sint<T>, rsint_exact<sizeof(T)>, ruint_exact<sizeof(T)>.
    template <Aint T> using raint_of = typename detail::raint_of_t<T>::type;

    // Safely construct an integer of a Target type from an arbitrary literal.
    template <Aint Target>
    consteval auto aint_from_literal(auto val) noexcept -> Target
    {
        constexpr bool v_signed = Sint<decltype(val)>;
        constexpr bool t_signed = Sint<Target>;

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

    template <Aint Target>
    constexpr auto aint_saturating_cast(auto val) noexcept -> Target
    {
        using V = decltype(val);
        constexpr auto v_signed = Sint<decltype(val)>;
        constexpr auto t_signed = Sint<Target>;

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
