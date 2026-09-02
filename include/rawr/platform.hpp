//// rawr/platform.hpp.

#ifdef RAWR_MODULE
    export module rawr.platform;
    export import rawr.platform.linux;
#else
    #pragma once
    #include "rawr/platform/linux.hpp"
#endif
