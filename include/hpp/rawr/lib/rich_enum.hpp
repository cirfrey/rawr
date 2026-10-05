// GENERATED — do not edit
#pragma once


namespace rawr::inline lib
{
    template <typename E> concept RichEnum  = requires { requires E::_is_rawr_rich_enum; typename E::enum_type; };
    // NOTE: a rich_flags is also a rich_enum.
    template <typename E> concept RichFlags = requires { requires E::_is_rawr_rich_flags; typename E::enum_type; };

    namespace enum_trait
    {
        template <typename E> struct plain_enum    { using type = E; };
        template <RichEnum E> struct plain_enum<E> { using type = typename E::enum_type; };
    }
    template <typename E> using plain_enum = typename enum_trait::plain_enum<E>::type;
}
