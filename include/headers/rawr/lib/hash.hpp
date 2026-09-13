#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/hash.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.lib.hash;
    export import rawr.lib.hash.fnv1a;
#else
    #pragma once
    #include "rawr/lib/hash/fnv1a.hpp"
#endif
