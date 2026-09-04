#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/dist/todo.pp"
#endif
// Goal: Make it so annoying that you have no choice but to tackle the TODO.
// Usage: RAWR_TODO("Some todo here")
#pragma once

#define RAWR_TODO_STRINGIFY_(x) #x
#define RAWR_TODO_STRINGIFY(x) RAWR_TODO_STRINGIFY_(x)

// Since RAWR_COMPILER_MSVC is the only thing we need to check,
// we might as well make this header standalone with defined(_MSC_VER) instead.
#if defined(_MSC_VER)
    #define RAWR_TODO(msg) \
        __pragma(message(__FILE__ "(" RAWR_TODO_STRINGIFY(__LINE__) "): [TODO] " msg))
#else
    // Only emit warnings for the current file, sadly msvc doesnt support this.
    #if __INCLUDE_LEVEL__ == 1
        #define RAWR_TODO(msg) _Pragma(RAWR_TODO_STRINGIFY(GCC warning "[TODO] " msg))
    #else
        #define RAWR_TODO(x)
    #endif
#endif

#ifdef RAWR_NO_TODO
    #undef RAWR_TODO
    #define RAWR_TODO(x)
#endif
