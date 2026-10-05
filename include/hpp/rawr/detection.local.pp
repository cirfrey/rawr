// GENERATED — do not edit
// We could cheat and ask the build system to sneak this info to us
// ... but whats the fun in that?
// Also, we want this to be somewhat portable, why rely on others?
//
// All macros default to 0 so consumers can write:
//     #if RAWR_COMPILER_CLANG && !RAWR_PLATFORM_WINDOWS
// instead of:
//     #if defined(RAWR_COMPILER_CLANG) && !defined(RAWR_PLATFORM_WINDOWS)
//
// Composite flags are defined in #if blocks to allow things like RAWR_PP_IF(COMPOSITE, ...),
// If we naiively did
//     #define RAWR_IS_64BIT (RAWR_PTR_SIZE == 8)
// The following expansion would happen
//     RAWR_PP_IF(RAWR_IS_64BIT, ...) => RAWR_PP_IF_(4 == 8)
// That doesn't compile, instead we define it inside an #if block and the expansion
// behaves as you'd expect.
// It's a little more work on our end, but thats what we're here for, right?

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.detection.pp
#endif

#include "rawr/detection/abi.local.pp"
#include "rawr/detection/arch.local.pp"
#include "rawr/detection/bin.local.pp"
#include "rawr/detection/compiler.local.pp"
#include "rawr/detection/cxx_abi.local.pp"
#include "rawr/detection/cxx_version.local.pp"
#include "rawr/detection/endian.local.pp"
#include "rawr/detection/env.local.pp"
#include "rawr/detection/feature.local.pp"
#include "rawr/detection/platform.local.pp"
#include "rawr/detection/posix.local.pp"
#include "rawr/detection/ptr.local.pp"
#include "rawr/detection/san.local.pp"
