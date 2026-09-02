//// rawr/lib/intrin.hpp
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.lib.intrin.base;
    export import rawr.lib.intrin.math;
    export import rawr.lib.intrin.mem;
#else
    #include "rawr/lib/intrin/base.hpp"
    #include "rawr/lib/intrin/math.hpp"
    #include "rawr/lib/intrin/mem.hpp"
#endif
