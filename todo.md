
# Short term

[ ] rawr::sync
    - [X] detection stuff
    - [~] arch::X::atomic
    - [ ] guard<>
[ ] rawr::hw

## If theres time

[~] rawr::lib::typing.
[~] Generic test runner.
    - [ ] Also print environment on start -> detection.hpp -> renum

# Longer term

[ ] compile time signaling/error module.
[ ] rawr::maybe<T, typename NonValue=Nil> (encodes optional<> and expected<>)
[~] RAWR_TAGGED_UNION (ergonomic variant)
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
[ ] Incorporate stuff from intrin_handoff.hpp (?)
[ ] Flesh out
    - [X] rawr/data/linker_section
    - [X] rawr/data/environment
    - [ ] rawr/lib/dummy_return.hpp -> static_assert.hpp or similar
    - [ ] rawr/lib/endian.hpp
    - [X] rawr/lib/fmt.hpp
    - [X] rawr/lib/hash.hpp
    - [ ] rawr/lib/serialize.hpp (?)
[ ] rawr::lib::scoped_hook
[ ] Distribution
    - [X] module.manifest comment explaining format.
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
