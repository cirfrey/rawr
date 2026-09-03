
# Short term - 27/08/26 - 06/09/26

[ ] meson sanitizer integration
    - [ ] clang tidy the code
    - [ ] clang tidy workflow
    - [ ] cppcheck code/workflow
    - [ ] Clang Static Analyzer (CSA) code/workflow
[ ] rawr::test
    - [ ] linker section
    - [ ] write tests for rawr::test
    - [ ] write tests for the rest of the library.
[ ] Fix docker compiler explorer setup
[ ] rawr::sync
    - [X] detection stuff
    - [~] arch::X::atomic
    - [ ] guard<>
    - [ ] More complex primitives ?
[ ] Refactor rawr.lib.integer.strong into generated header -> better debug messages.
[ ] README.md
[ ] LICENSE.md
[ ] detection.pp: optional error on unknown arch/bin/etc
[ ] Distribution
    - [ ] Meson project version from rawr/lib/dist/version.(h)pp
[ ] Review linux syscall code for platform/compiler/arch correctness and gating.

# Longer term

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

[ ] rawr::lib::numeric
    - [~] rawr::lib::numeric::strong_integer -> needs ::numeric
    - [ ] rawr::lib::numeric::strong_fixed
    - [ ] rawr::lib::numeric::strong_floating
[ ] review soft:: performance across the board.
[ ] rawr::lib::array
[ ] more abis, archs, platforms, bins, etc.
[ ] rawr.lib.libc
