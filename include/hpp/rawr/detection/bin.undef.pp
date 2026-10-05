// GENERATED — do not edit
#if defined(__ELF__)
    #undef RAWR_BIN_ELF
#elif defined(__MACH__)
    #undef RAWR_BIN_MACHO
#elif defined(_WIN32)
    #undef RAWR_BIN_PE
#elif RAWR_PLATFORM_WASM
    #undef RAWR_BIN_WASM
#else
    #undef RAWR_BIN_UNKNOWN
#endif
#undef RAWR_BIN_UNKNOWN
#undef RAWR_BIN_WASM
#undef RAWR_BIN_PE
#undef RAWR_BIN_MACHO
#undef RAWR_BIN_ELF
#include "rawr/detection/platform.undef.pp"
#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
#endif
