//// rawr/lib/diag.hpp.

#ifdef RAWR_MODULE
    export module rawr.lib.diag;
    export import rawr.lib.diag.dwarf;
#else
    #pragma once
    #include "rawr/lib/diag/dwarf.hpp"
#endif
