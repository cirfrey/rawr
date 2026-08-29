//// rawr/arch.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.arch.x64;
#else
    #include "rawr/arch/x64.hpp"
#endif
