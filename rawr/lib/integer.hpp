//// rawr/lib/integer.hpp
#pragma region rawr/lib/integer.hpp

#ifndef RAWR_UNITY
    #pragma once
#endif

namespace rawr::inline lib
{
    template <typename T>
    concept uint =
        // 1. T(-1) > T(0) ensures it wraps around (is unsigned)
        // 2. Ensuring it's an integer by checking that division or modulo behaves discretely
        //    (or checking that it can't hold a fractional step like T(0.5))
        (T(-1) > T(0)) && (T(0.5) == T(0));

    template <typename T>
    concept sint =
        // 1. T(-1) < T(0) ensures it can represent negative numbers (signed)
        // 2. T(0.5) == T(0) ensures it is an integer type, screening out floats/doubles
        // 3. T(1) / T(2) == T(0) ensures it supports integer division truncation
        (T(-1) < T(0)) && (T(0.5) == T(0)) && (T(1) / T(2) == T(0));

    template <typename T>
    concept aint = uint<T> || sint<T>;

    constexpr auto uint_max(unsigned char bits) noexcept { return bits >= 64 ? ~0ull : (1ull << bits) - 1; }
    constexpr auto sint_max(unsigned char bits) noexcept { return static_cast<long long>(uint_max(bits - 1)); }
    constexpr auto sint_min(unsigned char bits) noexcept { return ~sint_max(bits); }

    template <aint T>
    constexpr T aint_max = T(-1) < T(0)
        ? static_cast<T>(sint_max(sizeof(T) * 8))
        : static_cast<T>(uint_max(sizeof(T) * 8));
    template <aint T>
    constexpr T aint_min = T(-1) < T(0)
        ? static_cast<T>(sint_min(sizeof(T) * 8))
        : T{0};

    namespace detail
    {
        // Correctly handles types with overlapping sizes. Choosing the first match.
        template <auto Bytes, typename... Types>
        struct select_by_size { static_assert(Bytes == 0, "No type of this size"); };

        template <auto Bytes, typename Type, typename... Rest>
        requires (sizeof(Type) == Bytes)
        struct select_by_size<Bytes, Type, Rest...> { using type = Type; };

        template <auto Bytes, typename Type, typename... Rest>
        struct select_by_size<Bytes, Type, Rest...> : select_by_size<Bytes, Rest...> {};

        template <unsigned long num_>
        consteval auto uint_capable()
        {
                 if constexpr(num_ > aint_max<unsigned long long>) static_assert(false, "Value is too large for any supported unsigned integer type");
            else if constexpr(num_ > aint_max<unsigned long>)      return static_cast<unsigned long long>(0);
            else if constexpr(num_ > aint_max<unsigned int>)       return static_cast<unsigned long>(0);
            else if constexpr(num_ > aint_max<unsigned short>)     return static_cast<unsigned int>(0);
            else if constexpr(num_ > aint_max<unsigned char>)      return static_cast<unsigned short>(0);
            else                                               return static_cast<unsigned char>(0);
        }
    }

    template <auto Bytes>         using uint_exact   = detail::select_by_size<Bytes, unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>::type;
    template <auto Bytes>         using sint_exact   = detail::select_by_size<Bytes,   signed char,   signed short,   signed int,   signed long,   signed long long>::type;
    template <unsigned long num_> using uint_capable = decltype(detail::uint_capable<num_>());

    // Safely construct an integer of a Target type from an arbitrary literal.
    template <aint Target>
    consteval auto lit(auto v) -> Target
    {
        using V = decltype(v);
        constexpr bool v_signed = V(-1) < V(0);
        constexpr bool r_signed = Target(-1) < Target(0);

        if constexpr (v_signed == r_signed) {
            if (v < aint_min<Target>)
                throw "Literal value underflows the target type's minimum limit.";
            if (v > aint_max<Target>)
                throw "Literal value overflows the target type's maximum limit.";
        }
        else if constexpr (v_signed && !r_signed) {
            if (v < 0)
                throw "Literal value is negative, which underflows this unsigned target type.";
            if (static_cast<unsigned long long>(v) < static_cast<unsigned long long>(aint_min<Target>))
                throw "Literal value underflows the target type's minimum limit.";
            if (static_cast<unsigned long long>(v) > static_cast<unsigned long long>(aint_max<Target>))
                throw "Literal value overflows the target type's maximum limit.";
        }
        else {
            // Case 3: Unsigned input 'v' to signed target 'R'
            if constexpr (aint_min<Target> >= 0) {
                if (static_cast<unsigned long long>(v) < static_cast<unsigned long long>(aint_min<Target>))
                    throw "Literal value underflows the target type's minimum limit.";
            }

            if constexpr (aint_max<Target> < 0) {
                throw "Literal value overflows the target type's maximum limit.";
            } else {
                if (static_cast<unsigned long long>(v) > static_cast<unsigned long long>(aint_max<Target>))
                    throw "Literal value overflows the target type's maximum limit.";
            }
        }

        return static_cast<Target>(v);
    }
}
#pragma endregion rawr/lib/integer.hpp
