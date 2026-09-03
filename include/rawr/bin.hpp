//// rawr/bin.hpp.

#ifdef RAWR_MODULE
    export module rawr.bin;
    export import rawr.bin.elf;
#else
    #pragma once
    #include "rawr/bin/elf.hpp"
#endif
