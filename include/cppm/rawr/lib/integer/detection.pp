#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.lib.integer.detection.pp
#endif

#if defined(__SIZEOF_INT128__)
    #define RAWR_INTEGER_HAS_INT128 1
#else
    #define RAWR_INTEGER_HAS_INT128 0
#endif
