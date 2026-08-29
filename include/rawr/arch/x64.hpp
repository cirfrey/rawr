//// rawr/arch/x64.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.arch.x64.atomic;
    export import rawr.arch.x64.cpuid;
    export import rawr.arch.x64.simd;
#else
    #include "rawr/arch/x64/atomic.hpp"
    #include "rawr/arch/x64/cpuid.hpp"
    #include "rawr/arch/x64/simd.hpp"
#endif
