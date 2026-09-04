
#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/sync/base.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.lib.sync.base;
    import rawr.lib.integer.raw;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/raw.hpp"

    #include "rawr/lib/dist/header.pp"
#endif

RAWR_EXPORT namespace rawr::inline lib::inline sync
{
    enum class memory_order : ru8
    {
        relaxed,
        acquire,
        release,
        acq_rel,
        seq_cst,
    };
}
