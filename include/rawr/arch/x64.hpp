#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/arch/x64.hpp"
#endif

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
