
//// rawr/lib/sync/base.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    import rawr.lib.integer.raw;
#else
    #include "rawr/lib/integer/raw.hpp"
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
