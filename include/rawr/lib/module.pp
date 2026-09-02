#pragma once

#ifndef RAWR_MODULE
    #define RAWR_MODULE 0
#endif

#if RAWR_MODULE
    #define RAWR_EXPORT export
#else
    #define RAWR_EXPORT
#endif
