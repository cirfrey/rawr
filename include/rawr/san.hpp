#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/san.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.san;
    export import rawr.san.asan;
    export import rawr.san.lsan;
    export import rawr.san.msan;
    export import rawr.san.tsan;
#else
    #pragma once
    #include "rawr/san/asan.hpp"
    #include "rawr/san/lsan.hpp"
    #include "rawr/san/msan.hpp"
    #include "rawr/san/tsan.hpp"
#endif
#include "rawr/san/attributes.pp"
