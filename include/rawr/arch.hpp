//// rawr/arch.hpp.

#ifdef RAWR_MODULE
    export module rawr.arch;
    export import rawr.arch.x64;
#else
    #pragma once
    #include "rawr/arch/x64.hpp"
#endif
