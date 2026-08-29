//// rawr/bin.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.bin.elf;
#else
    #include "rawr/bin/elf.hpp"
#endif
