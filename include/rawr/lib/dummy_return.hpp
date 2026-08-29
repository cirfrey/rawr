#pragma once

#include "rawr/lib/module.pp"

RAWR_EXPORT namespace rawr::inline lib
{
    struct dummy_return {
        template <typename T> operator T() const noexcept;
    };
}
