// GENERATED — do not edit
#pragma once


namespace rawr::inline lib::linker_section::msvc
{
    template <typename PointerType>
    struct iterator
    {
        PointerType const* current;
        PointerType const* end;

        constexpr void skip_nulls() noexcept
        { while (current != end && *current == nullptr) ++current; }

        constexpr auto operator*() const noexcept -> decltype(**current)
        { return **current; }

        constexpr auto operator->() const noexcept -> decltype(*current)
        { return *current; }

        constexpr auto operator++() noexcept -> iterator&
        {
            ++current;
            skip_nulls();
            return *this;
        }

        constexpr auto operator++(int) noexcept -> iterator
        {
            auto copy = *this;
            ++*this;
            return copy;
        }

        friend constexpr auto operator==(iterator a, iterator b) noexcept -> bool
        { return a.current == b.current; }
    };

    template<decltype(sizeof(0)) N>
    struct fixed_string
    {
        char value[N];

        consteval fixed_string(char const (&str)[N])
        {
            for (decltype(N) i = 0; i < N; ++i) value[i] = str[i];
        }
    };

    template<decltype(sizeof(0)) N>
    fixed_string(char const (&)[N]) -> fixed_string<N>;

    template<fixed_string File, decltype(sizeof(0)) Counter>
    struct linker_anchor
    {
        static void fn();
    };
}
