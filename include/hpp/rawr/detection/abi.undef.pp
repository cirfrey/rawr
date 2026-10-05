// GENERATED — do not edit
#if RAWR_ARCH_WASM
    #undef RAWR_ABI_WASM
#elif RAWR_ARCH_X64
    #if RAWR_PLATFORM_WINDOWS
        #undef RAWR_ABI_WIN64
    #else
        #undef RAWR_ABI_SYSV
    #endif
#elif RAWR_ARCH_ARM64
    #if RAWR_PLATFORM_MACOS || RAWR_PLATFORM_IOS
        #undef RAWR_ABI_AAPCS64_APPLE
    #else
        #undef RAWR_ABI_AAPCS64
    #endif
#elif RAWR_ARCH_ARM32
    #undef RAWR_ABI_AAPCS32
#elif RAWR_ARCH_RISCV64
    #undef RAWR_ABI_RISCV_LP64
#elif RAWR_ARCH_RISCV32
    #undef RAWR_ABI_RISCV_ILP32
#elif RAWR_ARCH_XTENSA
    #undef RAWR_ABI_XTENSA
#elif RAWR_ARCH_AVR
    #undef RAWR_ABI_AVR
#else
    #undef RAWR_ABI_UNKNOWN
#endif
#undef RAWR_ABI_UNKNOWN
#undef RAWR_ABI_WASM
#undef RAWR_ABI_AVR
#undef RAWR_ABI_XTENSA
#undef RAWR_ABI_RISCV_ILP32
#undef RAWR_ABI_RISCV_LP64
#undef RAWR_ABI_AAPCS32
#undef RAWR_ABI_AAPCS64_APPLE
#undef RAWR_ABI_AAPCS64
#undef RAWR_ABI_WIN64
#undef RAWR_ABI_SYSV
#include "rawr/distribution/todo.undef.pp"
#include "rawr/detection/platform.undef.pp"
#include "rawr/detection/arch.undef.pp"
#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
#endif
