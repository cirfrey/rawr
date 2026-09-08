#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/intrin/base.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.lib.intrin.base;
    import rawr.lib.bits;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/bits.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"
#include "rawr/lib/attributes.pp"
#include "rawr/lib/compiler.pp"

RAWR_EXPORT namespace rawr::inline lib::intrin::inline base
{
    template <typename T> T&& declval() noexcept;

    namespace soft
    {
        template <typename T, typename U> struct is_same       { static constexpr auto value = false; };
        template <typename T>             struct is_same<T, T> { static constexpr auto value = true; };

        template <typename From, typename To>
        concept ImplictlyConvertible = requires {
            // This tests if an expression of type 'From' can be passed
            // to a function expecting 'To' (implicit conversion)
            void(declval<void(*)(To)>()(declval<From>()));
        };

        template <typename T> struct remove_cvref                   { using type = T; };
        template <typename T> struct remove_cvref<const T>          { using type = T; };
        template <typename T> struct remove_cvref<volatile T>       { using type = T; };
        template <typename T> struct remove_cvref<const volatile T> { using type = T; };
        template <typename T> struct remove_cvref<T&>  : remove_cvref<T> {};
        template <typename T> struct remove_cvref<T&&> : remove_cvref<T> {};
    }

    template <typename T>
    using bare =
        #if RAWR_COMPILER_MSVC || \
            (RAWR_COMPILER_CLANG && RAWR_COMPILER_VERSION_MAJOR < 16) || \
            (RAWR_COMPILER_GCC && RAWR_COMPILER_VERSION_MAJOR < 13)
            typename soft::remove_cvref<T>::type;
        #else
            __remove_cvref(T);
        #endif

    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto is_consteval() noexcept -> bool
    { return __builtin_is_constant_evaluated(); }

    // MSVC is quite picky with __is_same.
    template<typename T, typename... Us> concept is = (
        RAWR_MSVC(soft::is_same<T, Us>::value) RAWR_NOT_MSVC(__is_same(T, Us))
        || ...
    );

    template <typename T> concept TriviallyCopyable = __is_trivially_copyable(T);
    template <typename T> concept StandardLayout    = __is_standard_layout(T);

    #if RAWR_COMPILER_CLANG || (RAWR_COMPILER_GCC && RAWR_COMPILER_VERSION_MAJOR >= 13)
        template <typename From, typename To>
        concept ConvertibleTo = __is_convertible(From, To) && requires { static_cast<To>(declval<From>()); };
    #elif RAWR_COMPILER_MSVC
        template <typename From, typename To>
        concept ConvertibleTo = __is_convertible_to(From, To) && requires { static_cast<To>(declval<From>()); };
    #else
        template <typename From, typename To>
        concept ConvertibleTo = soft::ImplictlyConvertible<From, To> && requires { static_cast<To>(declval<From>()); };
    #endif

    template <TriviallyCopyable To, TriviallyCopyable From>
    requires (sizeof(To) == sizeof(From))
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto bit_cast(From const& from) noexcept -> To
    { return __builtin_bit_cast(To, from); }

    template <TriviallyCopyable To, TriviallyCopyable From>
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
