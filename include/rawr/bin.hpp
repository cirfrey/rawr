#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/bin.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.bin;
    export import rawr.bin.elf;
#else
    #pragma once
    #include "rawr/bin/elf.hpp"
#endif
