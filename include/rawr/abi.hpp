#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/abi.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.abi;
    export import rawr.abi.sysv;
    export import rawr.abi.win64;
#else
    #pragma once
    #include "rawr/abi/sysv.hpp"
    #include "rawr/abi/win64.hpp"
#endif
#include "rawr/abi/win64.pp"
#include "rawr/abi/sysv.pp"
