
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
[ ] Refactor rawr.lib.intrin into "if constexpr" based dispatch (gnu:: and msvc:: always declared)
[ ] README.md
[ ] LICENSE.md
[ ] Distribution
    - [X] Amalgam script.
    - [X] Amalgam target.
    - [ ] Amalgam on every commit.
    - [X] Eliminate .cppm (inject -DRAWR_MODULE via command line)
    - [X] scan-deps working with folders
    - [X] scan-deps not choking on .pp and .hpp
    - [ ] Meson project version from rawr/lib/dist/version.(h)pp
    - [X] pp_transitives.pp -> dist/pp.pp
    - [X] module.pp         -> dist/cppm.pp + dist/hpp.pp
    - [X] module.manifest in the source tree.
    - [X] module.manifest on every build.
    - [ ] module.manifest on every commit.

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
