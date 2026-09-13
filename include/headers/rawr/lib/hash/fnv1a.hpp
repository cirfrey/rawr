#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/hash/fnv1a.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.lib.hash.fnv1a;
    import rawr.lib.integer.raw;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/raw.hpp"

    #include "rawr/lib/dist/header.pp"
#endif

RAWR_EXPORT namespace rawr::inline lib::inline hash::fnv1a
{
    inline constexpr ru32 offset_basis_32 = 0x811c9dc5u;
    inline constexpr ru32 prime_32        = 0x01000193u;

    inline constexpr ru64 offset_basis_64 = 0xcbf29ce484222325ull;
    inline constexpr ru64 prime_64        = 0x100000001b3ull;

    template <typename T>
    constexpr auto hash32(T const* data, rst size) noexcept -> ru32
    requires(sizeof(T) == 1)
    {
        auto hash = offset_basis_32;
        for (rst i = 0; i < size; ++i) {
            hash ^= ru32{ static_cast<ru8>(data[i]) };
            hash *= prime_32;
        }
        return hash;
    }

    template <typename T>
    constexpr auto hash64(T const* data, rst size) noexcept -> ru64
    requires(sizeof(T) == 1)
    {
        auto hash = offset_basis_64;
        for (rst i = 0; i < size; ++i) {
            hash ^= ru64{ static_cast<ru8>(data[i]) };
            hash *= prime_64;
        }
        return hash;
    }

    inline namespace literals
    {
        // NOTE: These do NOT include the null-terminator in the hash. Be wary of comparing against hashes of null-terminated strings.
        consteval auto operator ""_fnv1a32(char const* str, rst len) -> ru32 { return hash32(str, len); }
        consteval auto operator ""_fnv1a64(char const* str, rst len) -> ru64 { return hash64(str, len); }

        // NOTE: These DO include the null-terminator in the hash.
        consteval auto operator ""_fnv1a32_nt(char const* str, rst len) -> ru32 { return hash32(str, len + 1); }
        consteval auto operator ""_fnv1a64_nt(char const* str, rst len) -> ru64 { return hash64(str, len + 1); }
    }
}
