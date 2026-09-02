//// rawr/lib/intrin.hpp.

#ifdef RAWR_MODULE
    export module rawr.lib.intrin;
    export import rawr.lib.intrin.base;
    export import rawr.lib.intrin.math;
    export import rawr.lib.intrin.mem;
#else
    #pragma once
    #include "rawr/lib/intrin/base.hpp"
    #include "rawr/lib/intrin/math.hpp"
    #include "rawr/lib/intrin/mem.hpp"
#endif
