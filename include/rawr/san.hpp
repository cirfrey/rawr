//// rawr/san.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.san.asan;
    export import rawr.san.lsan;
    export import rawr.san.msan;
    export import rawr.san.tsan;
#else
    #include "rawr/san/asan.hpp"
    #include "rawr/san/lsan.hpp"
    #include "rawr/san/msan.hpp"
    #include "rawr/san/tsan.hpp"
#endif
#include "rawr/san/attributes.pp"
