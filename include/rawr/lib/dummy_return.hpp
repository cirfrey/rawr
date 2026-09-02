//// rawr/lib/dummy_return.hpp.

#ifdef RAWR_MODULE
    export module rawr.lib.dummy_return;
    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/dist/header.pp"
#endif

RAWR_EXPORT namespace rawr::inline lib
{
    struct dummy_return {
        template <typename T> operator T() const noexcept;
    };
}
