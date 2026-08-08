
//// rawr/data/aliases.hpp
#pragma region rawr/data/aliases.hpp

#ifndef RAWR_UNITY
    #pragma once
    #include "rawr/lib/integer.hpp"
#endif

namespace rawr::inline data::inline aliases
{
    /// These represent fixed sizes, regardless of the platform you are
    /// compiling in. Use them when you need to store a specific number of bits.

    using u8  = lib::uint_exact<1>; using s8  = lib::sint_exact<1>;
    using u16 = lib::uint_exact<2>; using s16 = lib::sint_exact<2>;
    using u32 = lib::uint_exact<4>; using s32 = lib::sint_exact<4>;
    using u64 = lib::uint_exact<8>; using s64 = lib::sint_exact<8>;
    using f32 = float;
    using f64 = double;
    static_assert(sizeof(f32) == 32/8 && sizeof(f64) == 64/8);

    /// These are plarform specific, be wary of using them across platforms.
    /// Use them to encode information specific to this_platform.

    // Our very own free-range std::size_t.
    using st  = decltype(sizeof(0));
    // Corresponds to std::intptr_t.
    using ipt = decltype(static_cast<char*>(nullptr) - static_cast<char*>(nullptr));
    // Corresponds to std::uintptr_t.
    using upt = lib::uint_exact<sizeof(ipt)>;

    inline namespace literals
    {
        constexpr auto operator""_u8(unsigned long long val)  { return static_cast<u8>(val); }
        constexpr auto operator""_u16(unsigned long long val) { return static_cast<u16>(val); }
        constexpr auto operator""_u32(unsigned long long val) { return static_cast<u32>(val); }
        constexpr auto operator""_u64(unsigned long long val) { return static_cast<u64>(val); }

        constexpr auto operator""_s8(unsigned long long val)  { return static_cast<s8>(val); }
        constexpr auto operator""_s16(unsigned long long val) { return static_cast<s16>(val); }
        constexpr auto operator""_s32(unsigned long long val) { return static_cast<s32>(val); }
        constexpr auto operator""_s64(unsigned long long val) { return static_cast<s64>(val); }

        constexpr auto operator""_st(unsigned long long val)  { return static_cast<st>(val); }
        constexpr auto operator""_ipt(unsigned long long val) { return static_cast<ipt>(val); }
        constexpr auto operator""_upt(unsigned long long val) { return static_cast<upt>(val); }
    };
}

#pragma endregion rawr/data/aliases.hpp
