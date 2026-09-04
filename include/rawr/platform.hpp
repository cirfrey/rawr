#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/platform.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.platform;
    export import rawr.platform.linux;
#else
    #pragma once
    #include "rawr/platform/linux.hpp"
#endif
