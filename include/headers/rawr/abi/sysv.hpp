#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/abi/sysv.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.abi.sysv;
    export import rawr.abi.sysv.ctx;
#else
    #pragma once
    #include "rawr/abi/sysv/ctx.hpp"
#endif
#include "rawr/abi/sysv/main.pp"
#include "rawr/abi/sysv/trampolines.pp"
