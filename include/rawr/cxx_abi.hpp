//// rawr/cxx_abi.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.cxx_abi.itanium;
#else
    #include "rawr/cxx_abi/itanium.hpp"
#endif
