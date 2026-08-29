//// rawr/lib/integer.hpp
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.lib.integer.base;
    export import rawr.lib.integer.raw;
    export import rawr.lib.integer.strong;
#else
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/integer/strong.hpp"
#endif
