//// rawr/lib/integer.hpp.

#ifdef RAWR_MODULE
    export module rawr.lib.integer;
    export import rawr.lib.integer.base;
    export import rawr.lib.integer.raw;
    export import rawr.lib.integer.strong;
#else
    #pragma once
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/integer/strong.hpp"
#endif
