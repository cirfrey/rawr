[ ] rawr::lib::simd (semantic)
- [ ] rawr::lib::simd::dispatch
- [ ] rawr::lib::simd::profile: cpuid -> profile -> dispatch<profile[]>
[X] rawr::data::bitfield optimizations
- [X] rawr::pp optimizations
[ ] TLS stuff
[x] rawr::san - Basic version implemented.
[ ] fix rawr::abi::microsoft (teb)
- [ ] rawr::data::ptr<>
- [ ] rawr::data::ref<>
[X] rawr::abi.hpp abi selector (RAWR_MAIN) with #error on non-detected include (like RAWR_ABI_SYSV_MAIN). -> rawr/lib/main.pp
[ ] fix local docker compiler-explorer
[~] meson
[ ] Incorporate stuff from intrin_handoff.hpp (?)
[ ] Flesh out
- [ ] rawr/data/linker_section
- [ ] rawr/data/environment
- [ ] rawr/lib/dummy_return.hpp -> static_assert.hpp or similar
- [ ] rawr/lib/endian.hpp
- [ ] rawr/lib/fmt.hpp
- [ ] rawr/lib/hash.hpp
- [ ] rawr/lib/serialize.hpp (?)
[ ] rawr::lib::scoped_hook
[x] reevaluate if ::data should be merged into ::lib
[x] evaluate if ::diag should go as ::lib::diag instead.
[x] evaluate if .cppm belong in src/

# longer term

[ ] rawr::lib::numeric
- [x] rawr::lib::numeric::strong_integer
- [ ] rawr::lib::numeric::strong_fixed
- [ ] rawr::lib::numeric::strong_floating
[ ] review soft:: performance across the board.
[ ] examples
[ ] rawr::data::array
[ ] more abis, archs, platforms, bins, etc.
[ ] rawr.lib.libc

## 27/08/26-31/08/26
[x] rawr::san
[] meson stuff
- [] sanitizers
    - [x] clang tidy the code
    - [] clang tidy workflow
    - [] cppcheck code/workflow
    - [] Clang Static Analyzer (CSA) code/workflow
- [] start sketching out testing for rawr
    - [] linker section
[] compiler explorer stuff
[] rawr::sync
- [] detection stuff -> claude
