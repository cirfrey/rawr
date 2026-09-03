//// rawr/lib/rich_enum.hpp.

#ifdef RAWR_MODULE
    export module rawr.lib.rich_enum;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once

    #include "rawr/lib/dist/header.pp"
#endif

RAWR_EXPORT namespace rawr::inline lib
{
    template <typename E> concept rich_enum  = requires { requires E::_is_rawr_rich_enum; typename E::enum_type; };
    // NOTE: a rich_flags is also a rich_enum.
    template <typename E> concept rich_flags = requires { requires E::_is_rawr_rich_flags; typename E::enum_type; };

    namespace enum_trait
    {
        template <typename E>  struct plain_enum    { using type = E; };
        template <rich_enum E> struct plain_enum<E> { using type = typename E::enum_type; };
    }
    template <typename E> using plain_enum = typename enum_trait::plain_enum<E>::type;
}
