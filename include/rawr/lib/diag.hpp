//// rawr/lib/diag.hpp
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.lib.diag.dwarf;
#else
    #include "rawr/lib/diag/dwarf.hpp"
#endif
