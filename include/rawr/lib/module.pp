#pragma once

#ifndef RAWR_MODULE
    #define RAWR_MODULE 0
#endif

#if RAWR_MODULE
    #define RAWR_EXPORT export
#else
    #define RAWR_EXPORT
#endif

#ifndef RAWR_PP_TRANSITIVE_AS_MODULE
    #define RAWR_PP_TRANSITIVE_AS_MODULE 1
#endif
#ifndef RAWR_PP_TRANSITIVE_AS_HEADER
    #define RAWR_PP_TRANSITIVE_AS_HEADER 0
#endif
