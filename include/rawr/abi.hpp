//// rawr/abi.hpp.

#ifdef RAWR_MODULE
    export module rawr.abi;
    export import rawr.abi.microsoft;
    export import rawr.abi.sysv;
#else
    #pragma once
    #include "rawr/abi/microsoft.hpp"
    #include "rawr/abi/sysv.hpp"
#endif
#include "rawr/abi/microsoft.pp"
#include "rawr/abi/sysv.pp"
