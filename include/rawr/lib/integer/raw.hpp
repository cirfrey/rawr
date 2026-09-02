//// rawr/lib/integer/raw.hpp.

#ifdef RAWR_MODULE
    export module rawr.lib.integer.raw;
    import rawr.lib.integer.base;
    import rawr.lib.bits;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/bits.hpp"

    #include "rawr/lib/dist/header.pp"
#endif

// Just in case you need the actual underlying type aliases. Prefer the strong
// wrapped versions instead.
RAWR_EXPORT namespace rawr::inline lib::inline integer::inline raw
{
    using ru8  = ruint_exact<biw8>;  using rs8  = rsint_exact<biw8>;
    using ru16 = ruint_exact<biw16>; using rs16 = rsint_exact<biw16>;
    using ru32 = ruint_exact<biw32>; using rs32 = rsint_exact<biw32>;
    using ru64 = ruint_exact<biw64>; using rs64 = rsint_exact<biw64>;
    using rf32 = float;
    using rf64 = double;
    static_assert(bitsof<rf32> == biw32 && bitsof<rf64> == biw64);
    // Our very own free-range std::size_t.
    using rst  = decltype(sizeof(0));
    // Corresponds to std::intptr_t.
    using ript = decltype(static_cast<char*>(nullptr) - static_cast<char*>(nullptr));
    // Corresponds to std::uintptr_t.
    using rupt = ruint_exact<bitsof<ript>>;

    inline namespace literals
    {
        constexpr auto operator""_ru8(unsigned long long val)  noexcept { return static_cast<ru8>(val);  }
        constexpr auto operator""_ru16(unsigned long long val) noexcept { return static_cast<ru16>(val); }
        constexpr auto operator""_ru32(unsigned long long val) noexcept { return static_cast<ru32>(val); }
        constexpr auto operator""_ru64(unsigned long long val) noexcept { return static_cast<ru64>(val); }
        constexpr auto operator""_rs8(unsigned long long val)  noexcept { return static_cast<rs8>(val);  }
        constexpr auto operator""_rs16(unsigned long long val) noexcept { return static_cast<rs16>(val); }
        constexpr auto operator""_rs32(unsigned long long val) noexcept { return static_cast<rs32>(val); }
        constexpr auto operator""_rs64(unsigned long long val) noexcept { return static_cast<rs64>(val); }
        constexpr auto operator""_rst(unsigned long long val)  noexcept { return static_cast<rst>(val);  }
        constexpr auto operator""_ript(unsigned long long val) noexcept { return static_cast<ript>(val); }
        constexpr auto operator""_rupt(unsigned long long val) noexcept { return static_cast<rupt>(val); }
    }
}
