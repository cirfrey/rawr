//// rawr/platform.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.platform.linux;
#else
    #include "rawr/platform/linux.hpp"
#endif
