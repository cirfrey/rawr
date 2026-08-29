//// rawr/abi.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.abi.microsoft;
    export import rawr.abi.sysv;
#else
    #include "rawr/abi/microsoft.hpp"
    #include "rawr/abi/sysv.hpp"
#endif
#include "rawr/abi/microsoft.pp"
#include "rawr/abi/sysv.pp"
