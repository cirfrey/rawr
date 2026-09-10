
# Short term - 07/09/26 - 13/09/26

[ ] rawr::sync
    - [X] detection stuff
    - [~] arch::X::atomic
    - [ ] guard<>
[ ] README.md
[ ] LICENSE.md
[ ] detection.pp: optional error on unknown arch/bin/etc
[ ] Distribution
    - [ ] Meson project version from rawr/lib/dist/version.(h)pp
[ ] Unify dist/module.pp and dist/header.pp into dist/hpp.pp.

## If theres time

[ ] merge attributes.pp into compiler.pp
[~] rawr::lib::typing.
[ ] Generic test runner.
    - [ ] Also print environment on start -> detection.hpp -> renum

# Longer term

[ ] Ergonomic compiletime testing.
[ ] rawr::test checks with messages.
[ ] Tokenized logging facilities.
    - [ ] Hashed source location -> linker section
    - [ ] Hashed string -> linker section
    - [ ] Hashed formatting -> linker section
        - [ ] str hash + send arguments binarily (encode arg info in linker section too?)
        - [ ] str hash + formatted argument string
    - [ ] firmware identity.
[ ] rawr::sync more complex primitives ?
[ ] meson sanitizer integration
    - [ ] clang tidy the code
    - [ ] clang tidy workflow
    - [ ] cppcheck code/workflow
    - [ ] Clang Static Analyzer (CSA) code/workflow
[ ] Fix docker compiler explorer setup
[ ] Review linux syscall code for platform/compiler/arch correctness and gating.
[ ] Refactor rawr.lib.integer.strong into generated header -> better debug messages.
[ ] write tests for the rest of the library.
[ ] examples
[ ] rawr::lib::simd (semantic)
    - [ ] rawr::lib::simd::dispatch
    - [ ] rawr::lib::simd::profile: cpuid -> profile -> dispatch<profile[ ]>
[ ] rawr::lib::tls stuff
[ ] fix rawr::abi::microsoft (teb)
    - [ ] rawr::data::ptr<>
    - [ ] rawr::data::ref<>
[ ] fix local docker compiler-explorer
[ ] Meson: setup headeronly.
[ ] Incorporate stuff from intrin_handoff.hpp (?)
[ ] Flesh out
    - [ ] rawr/data/linker_section
    - [ ] rawr/data/environment
    - [ ] rawr/lib/dummy_return.hpp -> static_assert.hpp or similar
    - [ ] rawr/lib/endian.hpp
    - [X] rawr/lib/fmt.hpp
    - [ ] rawr/lib/hash.hpp
    - [ ] rawr/lib/serialize.hpp (?)
[ ] rawr::lib::scoped_hook
[ ] Distribution
    - [ ] module.manifest comment explaining format.
    - [ ] CMake.
    - [ ] Integrate with package managers.

# Even longer term

[ ] replace clang-scan-deps with bespoke script
    - [ ] Also validates that #ifdef RAWR_MODULE import/include lists match.
[ ] rawr::lib::numeric
    - [~] rawr::lib::numeric::strong_integer -> needs ::numeric
    - [ ] rawr::lib::numeric::strong_fixed
    - [ ] rawr::lib::numeric::strong_floating
[ ] review soft:: performance across the board.
[ ] rawr::lib::array
[ ] more abis, archs, platforms, bins, etc.
[ ] rawr.lib.libc
