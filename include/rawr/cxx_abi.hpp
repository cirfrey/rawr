//// rawr/cxx_abi.hpp.

#ifdef RAWR_MODULE
    export module rawr.cxx_abi;
    export import rawr.cxx_abi.itanium;
#else
    #pragma once
    #include "rawr/cxx_abi/itanium.hpp"
#endif
