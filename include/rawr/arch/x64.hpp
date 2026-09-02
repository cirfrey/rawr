//// rawr/arch/x64.hpp.

#ifdef RAWR_MODULE
    export module rawr.arch.x64;
    export import rawr.arch.x64.atomic;
    export import rawr.arch.x64.cpuid;
    export import rawr.arch.x64.simd;
#else
    #pragma once
    #include "rawr/arch/x64/atomic.hpp"
    #include "rawr/arch/x64/cpuid.hpp"
    #include "rawr/arch/x64/simd.hpp"
#endif
