//// rawr/bin/elf.hpp.

#ifdef RAWR_MODULE
    export module rawr.bin.elf;
    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/attributes.pp"

// In a static binary with a proper linker script these symbols always exist.
// The empty case is already covered by start == end. Null checks produce extra instructions
// and in PIC mode extra GOT reads for a condition that never occurs.
// Removing __attribute__((weak)) simplifies the output with no practical downside.
//
// Adding __attribute__((visibility("hidden"))) tells the compiler
// that this symbol will not be overridden by a dynamic library.
// This allows the compiler to optimize out GOT lookups entirely.
// NOLINTBEGIN(bugprone-reserved-identifier)
RAWR_EXPORT namespace rawr::bin::elf::symbols
{
    // Static globals that require runtime initialization end up here.
    namespace init_array
    {
        using callback_t = void(*)();
        extern "C" callback_t __init_array_start[] RAWR_HIDDEN;
        extern "C" callback_t __init_array_end[]   RAWR_HIDDEN;
        inline constexpr auto& start = __init_array_start;
        inline constexpr auto& end   = __init_array_end;
    }

    // Things marked with __attribute__((destructor)) end up here.
    // If you are using these directly: Don't forget you must traverse fini backwards.
    namespace fini_array
    {
        using callback_t = void(*)();
        extern "C" callback_t __fini_array_start[] RAWR_HIDDEN;
        extern "C" callback_t __fini_array_end[]   RAWR_HIDDEN;
        inline constexpr auto& start = __fini_array_start;
        inline constexpr auto& end   = __fini_array_end;
    }

    // Opaque DSO identity required by the Itanium C++ ABI.
    // The compiler passes &__dso_handle as the third argument to __cxa_atexit.
    namespace dso
    {
        extern "C" void* __dso_handle;
        inline constexpr auto& handle = __dso_handle;
        // The extern "C" in the macro is necessary because the declaration has C linkage and the definition must match.
        #define RAWR_BIN_ELF_DEFINE_DSO() extern "C" { void* __dso_handle = &::rawr::bin::elf::symbols::dso::__dso_handle; }
    }
}
// NOLINTEND(bugprone-reserved-identifier)

// Some commmon functions over these symbols.
RAWR_EXPORT namespace rawr::bin::elf {
    // Just an alias for doing the operation directly.
    [[gnu::always_inline]] inline auto static_init() noexcept -> void {
        namespace init = symbols::init_array;
        // An extern object is assumed to have an address. The compiler assumes the symbol will be resolved at link time.
        // If the linker fails to find the symbol, the link step fails, not the runtime. Runtime null checks are therefore
        // checking for a condition that is impossible if your binary successfully linked.
        // Therefore the following is unnecessary:
        //   if (!start || !end) return;
        for(auto* callback = init::start; callback != init::end; ++callback) { (*callback)(); }
    }
}
