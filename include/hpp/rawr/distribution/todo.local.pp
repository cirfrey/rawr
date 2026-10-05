// GENERATED — do not edit
// Goal: Make it so annoying that you have no choice but to tackle the TODO.
// Usage: RAWR_TODO("Some todo here")

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.distribution.todo.pp
#endif

#undef RAWR_TODO_STRINGIFY_
#define RAWR_TODO_STRINGIFY_(x) #x
#undef RAWR_TODO_STRINGIFY
#define RAWR_TODO_STRINGIFY(x) RAWR_TODO_STRINGIFY_(x)

// Since RAWR_COMPILER_MSVC is the only thing we need to check,
// we might as well make this header standalone with defined(_MSC_VER) instead.
#if defined(_MSC_VER)
    #undef RAWR_TODO
    #define RAWR_TODO(msg) \
        __pragma(message(__FILE__ "(" RAWR_TODO_STRINGIFY(__LINE__) "): [TODO] " msg))
#else
    // Only emit warnings for the current file, sadly msvc doesnt support this.
    #ifndef RAWR_TODO_INCLUDELEVEL
        #undef RAWR_TODO_INCLUDELEVEL
        #define RAWR_TODO_INCLUDELEVEL 1
    #endif

    #if __INCLUDE_LEVEL__ == RAWR_TODO_INCLUDELEVEL
        #undef RAWR_TODO
        #define RAWR_TODO(msg) _Pragma(RAWR_TODO_STRINGIFY(GCC warning "[TODO] " msg))
    #else
        #undef RAWR_TODO
        #define RAWR_TODO(x)
    #endif
#endif

#ifdef RAWR_NO_TODO
    #undef RAWR_TODO
    #define RAWR_TODO(x)
#endif
