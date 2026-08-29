//// rawr/lib/sync.hpp
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    export import rawr.lib.sync.base;
#else
    #include "rawr/lib/sync/base.hpp"
#endif
