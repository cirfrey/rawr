//// rawr.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.abi;
    export import rawr.arch;
    export import rawr.bin;
    export import rawr.cxx_abi;
    export import rawr.lib;
    export import rawr.platform;
    export import rawr.san;
#else
    #include "rawr/abi.hpp"
    #include "rawr/arch.hpp"
    #include "rawr/bin.hpp"
    #include "rawr/cxx_abi.hpp"
    #include "rawr/lib.hpp"
    #include "rawr/platform.hpp"
    #include "rawr/san.hpp"
#endif
