//// rawr/lib/source_location.hpp.

#ifdef RAWR_MODULE
    export module rawr.lib.source_location;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"

RAWR_EXPORT namespace rawr::inline lib
{
    struct source_location {
        char const* file     = nullptr;
        char const* function = nullptr;
        unsigned    line     = 0;
        unsigned    column   = 0;

        static constexpr source_location current(
            const char* file = __builtin_FILE(),
            const char* func = __builtin_FUNCTION(),
            unsigned    line = __builtin_LINE(),
            #if RAWR_COMPILER_GCC
                unsigned    col = 0
            #else
                unsigned    col = __builtin_COLUMN()
            #endif
        ) noexcept { return source_location{file, func, line, col}; }
    };
}
