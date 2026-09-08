#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/sass.hpp"
#endif
// Static assertion utils.

#ifdef RAWR_MODULE
    export module rawr.lib.sass;
    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/dist/header.pp"
#endif

RAWR_EXPORT namespace rawr::inline lib::sass
{
    template <typename...>
    inline constexpr auto fail = false;
}
