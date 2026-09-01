//// rawr/lib/bits.hpp.
#pragma once

#include "rawr/lib/module.pp"

RAWR_EXPORT namespace rawr::inline lib::inline bits
{
    inline constexpr auto bits_in_byte = 8; // TODO: detect.

    struct bitwidth {
        decltype(sizeof(0)) val{};
        [[nodiscard]] constexpr auto is_zero() const noexcept -> bool { return val == 0; }
    };

    struct bytewidth {
        decltype(sizeof(0)) val{};
        [[nodiscard]] constexpr auto is_zero() const noexcept -> bool { return val == 0; }

        constexpr operator bitwidth() const noexcept { return bitwidth{val * bits_in_byte}; }
    };

    constexpr auto operator>(bitwidth  const lhs, bitwidth const rhs) -> bool { return lhs.val > rhs.val; }
    constexpr auto operator<(bitwidth  const lhs, bitwidth const rhs) -> bool { return lhs.val < rhs.val; }
    constexpr auto operator==(bitwidth const lhs, bitwidth const rhs) -> bool { return lhs.val == rhs.val; }

    inline namespace literals
    {
        constexpr auto operator""_biw(unsigned long long val) noexcept { return bitwidth{val}; }
        constexpr auto operator""_byw(unsigned long long val) noexcept { return bytewidth{val}; }
    }

    inline constexpr auto biw0   = 0_biw;
    inline constexpr auto biw8   = 8_biw;
    inline constexpr auto biw16  = 16_biw;
    inline constexpr auto biw32  = 32_biw;
    inline constexpr auto biw64  = 64_biw;
    inline constexpr auto biw128 = 128_biw;
    inline constexpr auto byw0   = 0_byw;
    inline constexpr auto byw1   = 1_byw;
    inline constexpr auto byw2   = 2_byw;
    inline constexpr auto byw4   = 4_byw;
    inline constexpr auto byw8   = 8_byw;
    inline constexpr auto byw16  = 16_byw;

    template <typename T> inline constexpr auto bitsof = bitwidth{ sizeof(T) * bits_in_byte };
    template <typename T>        constexpr auto bitsofe([[maybe_unused]] T&& expr) noexcept { return bitsof<T>; }

    template <typename T> inline constexpr auto bytesof = bytewidth{ sizeof(T) };
    template <typename T>        constexpr auto bytesofe([[maybe_unused]] T&& expr) noexcept { return bytesof<T>; }

    enum class byte : unsigned char {};
}
