//// rawr/lib.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.lib.bitfield;
    export import rawr.lib.bits;
    export import rawr.lib.detection;
    export import rawr.lib.diag;
    export import rawr.lib.dummy_return;
    export import rawr.lib.integer;
    export import rawr.lib.intrin;
    export import rawr.lib.rich_enum;
    export import rawr.lib.sync;
#else
    #include "rawr/lib/bitfield.hpp"
    #include "rawr/lib/bits.hpp"
    #include "rawr/lib/detection.hpp"
    #include "rawr/lib/diag.hpp"
    #include "rawr/lib/dummy_return.hpp"
    #include "rawr/lib/integer.hpp"
    #include "rawr/lib/intrin.hpp"
    #include "rawr/lib/rich_enum.hpp"
    #include "rawr/lib/sync.hpp"
#endif
#include "rawr/lib/attributes.pp"
#include "rawr/lib/bitfield.pp"
#include "rawr/lib/detection.pp"
#include "rawr/lib/main.pp"
#include "rawr/lib/module.pp"
#include "rawr/lib/pp_transitives.pp"
#include "rawr/lib/pp.pp"
#include "rawr/lib/rich_enum.pp"
