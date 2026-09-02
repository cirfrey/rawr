//// rawr/lib/intrin/base.hpp
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    import rawr.lib.bits;
#else
    #include "rawr/lib/bits.hpp"
#endif
#include "rawr/lib/detection.pp"
#include "rawr/lib/attributes.pp"

// MSVC is quite picky with __is_same.
#if RAWR_COMPILER_MSVC
    namespace rawr::inline lib::intrin::inline base::msvc
    {
        template <typename T, typename U> struct is_same       { static constexpr auto value = false; };
        template <typename T>             struct is_same<T, T> { static constexpr auto value = true; };
    }
#endif

RAWR_EXPORT namespace rawr::inline lib::intrin::inline base
{
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto is_consteval() noexcept -> bool
    { return __builtin_is_constant_evaluated(); }

    #if RAWR_COMPILER_MSVC
        template<typename T, typename... Us> concept is = (msvc::is_same<T, Us>::value || ...);
    #else
        template<typename T, typename... Us> concept is = (__is_same(T, Us) || ...);
    #endif

    template <typename T> concept is_trivially_copyable = __is_trivially_copyable(T);
    template <typename T> concept is_standard_layout    = __is_standard_layout(T);

    template <typename T> T&& declval() noexcept; // TODO: this doesnt belong here.
    #if RAWR_COMPILER_FAMILY_GNU
        template <typename From, typename To>
        concept convertible_to = __is_convertible(From, To) && requires { static_cast<To>(declval<From>()); };
    #elif RAWR_COMPILER_MSVC
        template <typename From, typename To>
        concept convertible_to = __is_convertible_to(From, To) && requires { static_cast<To>(declval<From>()); };
    #endif

    template <is_trivially_copyable To, is_trivially_copyable From>
    requires (sizeof(To) == sizeof(From))
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto bit_cast(From const& from) noexcept -> To
    { return __builtin_bit_cast(To, from); }

    template <is_trivially_copyable To, is_trivially_copyable From>
    requires (sizeof(To) < sizeof(From))
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto bit_cast(From const& from, unsigned char ByteOffset = 0) noexcept -> To
    {
        using src_bytes = byte_array<sizeof(From)>;
        using dst_bytes = byte_array<sizeof(To)>;

        auto const src = __builtin_bit_cast(src_bytes, from);
        dst_bytes dst{};

        for (decltype(sizeof(0)) i = 0; i < sizeof(To); ++i) {
            dst.data[i] = src.data[ByteOffset + i];
        }

        return __builtin_bit_cast(To, dst);
    }
}
