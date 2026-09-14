# rawr

Rawr is a from-scratch C++20 library. It is designed to be:

- a toolkit to assemble your own runtime;
- a deliberately unconventional systems programming library;
- an experiment into alternative / greenfield library design;
- a learning project and reference API;
- an exercise in how much can be done with just C++20 and some know-how.

It can be thought of as a library of utilities + explicit interfaces to the
otherwise invisible machinery beneath a C++ program, whatever that machinery
may be: ABI, compiler, platform, architecture, etc.

rawr is also intended to serve as the base layer for loom.a.ne, but rawr is
independently useful and is designed without making loom.a.ne's requirements
part of its API or architecture.

# Guidelines

rawr is designed to be what happens when you drop the cruft that comes from
expectations to conform to one or multiple existing standards (POSIX, libc,
what have you) and optimize purely for what is best/what you wish you had.

We make no assumptions about the running environment, memory layout, and
feature support: Bare metal should get the same respect that a hosted environment
does. Everything rawr must compile in freestanding mode, even the tests are
freestanding and use only rawr components.

You can use as much or as little of rawr as you want: rawr is designed to be as
non-viral as reasonable. Code is meant to compose, not inherit.

Our code has no obligation to be idiomatic or follow established guidelines.
We make decisions based on the following:

- Features are opt-in.
- Ownership is explicit.
- Non-zero-overhead features make their costs clear.
- Code is portable, correct, and correctly portable.
- Abstractions are cohesive, uniform, and non-redundant with other abstractions.

And not:

- Solve problems "how it's always been done".
- Feature parity or API similarity with libc.
- Being a general-purpose library. rawr is explicitly for systems programming, with the tradeoffs and benefits that come with that.

We will favor our own abstractions and conventions whenever they make more sense
for the library's goals, even in light of existing language features. Existing
conventions may be design guidelines/targets, but aren't authoritative.

## An example

A typical `RAWR_MAIN` consists of a minimal trampoline from `_start` (if needed on the platform) directly into your code.

And then you're free to decide:

- whether to run static initializers;
- whether to run static destruction on exit, and where its callbacks are stored;
- how exactly you're going to exit;
- whether to initialize TLS;
- whether to detect the supported SIMD facilities at runtime and dispatch appropriately, or just assume and hope it works;
- whether to setup buffered I/O, and how that's backed;
- or whether you want all of it taken care of in a more "standard" way, with the performance implications that come with it.

And of course, rawr provides facilities to make executing those decisions easy,
correct, and as performant as the environment allows, but it won't decide for you
(unless you tell it to).

# Status

Don't expect API stability or any sort of concreteness before 1.0.

The library is very early days and there's still very little in place, but the
abstractions are being designed to scale to the full scope of the library.

Things are implemented on an as-needed basis. The hypothetical full span of rawr
is very broad, and for now we're focusing on Linux/ELF and the library layer, with
Windows on the horizon.

Expanding platform coverage is often mechanically repetitive: more syscalls,
enumerations, intrinsics, ABI details, and things like that. Designing the
abstractions and patterns that make that repetitive work "just repetitive work"
is not.

The real meat of the design work is the library layer, which is meant to be
backend-agnostic while making more opinionated decisions about how systems
programming should work.

With that being said, rawr does compile on every supported compiler at its minimum
supported version:

- GCC   >= 11.1
- Clang >= 12.0.0
- MSVC  >= 19.28 (VS16.8)

Building also requires:

- meson  >= 1.10.0
- python >= 3.10

# Distribution

rawr has 3 distribution mechanisms:

- C++ modules: The main one, decisions around compilation
time and ease of use are made with that target
- C++ headers: Just add `include/headers` to your include path.
- single-header amalgam: Easy to vendor, see `include/amalgam`.

Compilation times are expected to be very slow compared when not using modules,
unless you invest into a precompiled header system.

In fact, you can test rawr directly in [Godbolt](https://godbolt.org/z/KPbP944KM)
using the amalgam. Again, don't expect fast compilation times.

```cpp
#include "https://raw.githubusercontent.com/cirfrey/rawr/refs/heads/main/include/amalgam/rawr.amalgam.hpp"

RAWR_MAIN_NOCTX
{
    namespace plat = rawr::platform::linux::x64;
    plat::syscall::write(plat::stdout, "Howdy, world!").discard();
    plat::syscall::exit(0);
}
```

## Build systems

Meson is currently the supported build system. CMake is possible/probable, but
it's as-needed, like everything else.

If you want to consume rawr as modules on an unsupported build system, we also
have a `module.manifest` that can be parsed manually to replicate module support
without scanning the source tree. That's how we do it for Meson. Here's a small
sample from `module.manifest`

```txt
# rawr module manifest v2
# path|module|requires|size|mtime_ns|sha256
include/headers/rawr/bin/elf.hpp|rawr.bin.elf||3093|1788553785796428900|6ce9c7065c145fbd39e90bad51adc1c9d9d2e620d67fe6edef83e22f14adecc7
include/headers/rawr/bin.hpp|rawr.bin|rawr.bin.elf|218|1788553785363947000|4ed26914f3b64f6a2019dc5d4408a944aa5c870d660544983416db21ed96c48e
include/headers/rawr/cxx_abi/itanium.hpp|rawr.cxx_abi.itanium||578|1788737480085615000|353dab41ccf8f5633940b5df74b5118e410f7ad9c882d2bb7e3eb32447d0cbcd
include/headers/rawr/cxx_abi.hpp|rawr.cxx_abi|rawr.cxx_abi.itanium|242|1788553785363947000|b6227a2d30aeb92b090f4aaf8d01a3b4e31260cd6a9b0849285a82c95d3a82e8
include/headers/rawr/lib/bits.hpp|rawr.lib.bits||2387|1788553785778653900|04da8a8298cf1d93c2d63382fcd2c54685bc2ca370384a6e6c5c774260c62079
include/headers/rawr/lib/dummy_return.hpp|rawr.lib.dummy_return||383|1788553785776161800|d635aebdf33e993a605f537e7f7256e9d499b2821c9d1d4ac8d4ec6bbc1d6b8e
include/headers/rawr/lib/integer/base.hpp|rawr.lib.integer.base|rawr.lib.bits|14817|1789059967165011200|9a6a782a7eb0b02ea9f1cc5bd7dc744f20a6d744bbc9b7faf793f7bf9c51ea8f
include/headers/rawr/lib/integer/raw.hpp|rawr.lib.integer.raw|rawr.lib.bits,rawr.lib.integer.base|2581|1789258218950141200|9b77c3dbf8d6177540931cf72fab2269ae7e624eba14f0a9696385b9221d621d
include/headers/rawr/abi/win64/ctx.hpp|rawr.abi.win64.ctx|rawr.lib.integer.raw|2537|1789407000887091700|de4e02439b06b90f4a3c869d333e4288ec185a77080223d88903875adf4c07ee
include/headers/rawr/abi/win64.hpp|rawr.abi.win64|rawr.abi.win64.ctx|277|1789407065023732800|c20eba09c342422d1e5101ae444992eafa15b71210e3588edadfd08c1ec3ba2a
include/headers/rawr/lib/detection.hpp|rawr.lib.detection|rawr.lib.integer.raw|9875|1788553785782458600|0cf5752daaab117ba720b4e73eb6c1b2a642b9067a8d72939b42920b13e3561f
include/headers/rawr/abi/sysv/ctx.hpp|rawr.abi.sysv.ctx|rawr.lib.detection,rawr.lib.integer.raw|16960|1789406109679310000|22d0a0e1bec89d9f6c89ec47dbc20e6809db6f5120e02703ab7cc1f117cbff21
include/headers/rawr/abi/sysv.hpp|rawr.abi.sysv|rawr.abi.sysv.ctx|313|1789405765591686800|fcf683daad6962e68aaa6b79ce12ff85f8c3ad6034554857e89cc5660057c2ce
include/headers/rawr/abi.hpp|rawr.abi|rawr.abi.sysv,rawr.abi.win64|290|1789407110854802200|9740dabc486ffba8b8feee94437a90d66b3c760d59708eb2aff996f729b130ef
include/headers/rawr/lib/hash/fnv1a.hpp|rawr.lib.hash.fnv1a|rawr.lib.integer.raw|1970|1789001042857984800|b580b2f0d44d9061b7090aa4c1e988fb86396260d255e9cee9c1f5b6b22ff9d5
include/headers/rawr/lib/hash.hpp|rawr.lib.hash|rawr.lib.hash.fnv1a|231|1788875458615438600|ca6b368a625e5f1336a5db2c0f58d4d4d49d0e59603285ee0714146bcb762ef1
include/headers/rawr/lib/intrin/base.hpp|rawr.lib.intrin.base|rawr.lib.bits|6545|1788989613396090700|7f0e1c9e04d555700f32facd28aa186851839e3acd10f60c9818da89e28ef9c8
```

## Macro files

When consuming rawr as modules or headers, the macros are in `.pp` files, this is because C++ modules do not export macros while headers so, so we need some uniform
way to consume macros in modules. Some macros are used internally by the modules,
while others are intended to be "exported" by the modules or used alongside them.

The `.pp` files, by default, only declare the macros. Many of those macros
reference code that may or may not be present at the point where the macro is
expanded (since the code has to be in a different file due to modules being
macro-hygienic).

For example:

```cpp
// Snipped from rawr/abi/sysv/main.pp
// Notice the ::rawr symbols refereced.

#define RAWR_ABI_SYSV_MAIN(...)                                                                            \
    static_assert(RAWR_ABI_SYSV_HAS_TRAMPOLINE, "SysV ctx trampoline not defined for this architecture" ); \
                                                                                                           \
    [[gnu::flatten]] RAWR_NORETURN RAWR_ALWAYS_INLINE auto rawr_user_main(__VA_ARGS__) noexcept -> void;   \
    extern "C" {                                                                                           \
        [[gnu::naked]] RAWR_NORETURN auto _start() noexcept -> void                                        \
        {                                                                                                  \
            __asm__(RAWR_ABI_SYSV_TRAMPOLINE);                                                             \
            RAWR_UNREACHABLE;                                                                              \
        }                                                                                                  \
                                                                                                           \
        [[gnu::flatten]] RAWR_NORETURN auto rawr_main(void* sp) noexcept -> void                           \
        {                                                                                                  \
            ::rawr::lib::diag::dwarf::mark_unwind_root();                                                  \
            auto ctx = ::rawr::abi::sysv::context64::from_stack_pointer(sp);                               \
            rawr_user_main(ctx);                                                                           \
        }                                                                                                  \
    }                                                                                                      \
    [[gnu::flatten]] RAWR_NORETURN RAWR_ALWAYS_INLINE auto rawr_user_main(__VA_ARGS__) noexcept -> void
```

If you want the `.pp` to bring in the relevant code as well, you need to tell it
to by defining either:
- `RAWR_PP_TRANSITIVE_AS_MODULE` or;
- `RAWR_PP_TRANSITIVE_AS_HEADER`.

```cpp
// Snipped from rawr/abi/sysv/main.pp, earlier.
// Notice how symbols are pulled in if you tell the .pp to do it.

#include "rawr/lib/dist/pp.pp"
#if RAWR_PP_TRANSITIVE_AS_MODULE
    import rawr.abi.sysv.ctx;
    import rawr.lib.diag.dwarf;
#endif
#if RAWR_PP_TRANSITIVE_AS_HEADER
    #include "rawr/abi/sysv/ctx.hpp"
    #include "rawr/lib/diag/dwarf.hpp"
#endif
#include "rawr/abi/sysv/trampolines.pp"
#include "rawr/lib/detection.pp"
#include "rawr/lib/attributes.pp"
```

This doesn't apply to the amalgam, you can just use it normally.

## Meson

Rawr's Meson integration exposes module dependencies through 3 interfaces:

- `rawr_dep`: the default dependency, is the whole project as a module
- `rawr_headeronly_dep`: rawr as headers.
- `rawr_amalgam_dep`: just the single-header amalgam.

You can also depend on specific modules by using the internal mod_dep dictionary:

```meson
rawr_abi_sysv_dep = mod_dep['rawr.abi.sysv']
```

TODO: mention scripts/tooling more indepth.

# Organization


# Module reference

This section will eventually document each module: what it does, what it depends on, what assumptions it makes, and any interesting details or design decisions that aren't obvious from the API itself.

TODO: defer to MODULES.md
