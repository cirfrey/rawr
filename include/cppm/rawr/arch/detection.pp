#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.arch.detection.pp
#endif

// --- Ids. ---

#define RAWR_ARCHID_UNKNOWN 1
#define RAWR_ARCHID_IA32    2
#define RAWR_ARCHID_X64     3
#define RAWR_ARCHID_ARM32   4
#define RAWR_ARCHID_ARM64   5
#define RAWR_ARCHID_RISCV32 6
#define RAWR_ARCHID_RISCV64 7
#define RAWR_ARCHID_XTENSA  8
#define RAWR_ARCHID_AVR     9
#define RAWR_ARCHID_WASM32  10
#define RAWR_ARCHID_WASM64  11
#define RAWR_ARCHID_PPC32   12
#define RAWR_ARCHID_PPC64   13
#define RAWR_ARCHID_LOONG64 14
#define RAWR_ARCHID_MIPS32  15
#define RAWR_ARCHID_MIPS64  16
#define RAWR_ARCHID_S390    17
#define RAWR_ARCHID_S390X   18
#define RAWR_ARCHID_MSP430  19
#define RAWR_ARCHID_SPARC32 20
#define RAWR_ARCHID_SPARC64 21
#define RAWR_ARCHID_SUPERH  22
#define RAWR_ARCHID_MAX     22

// --- Detection. ---

// WASM first — Emscripten defines __i386 for legacy reasons
#if defined(__wasm64__)
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_WASM64
#elif defined(__wasm32__) || defined(__wasm__)
    // Fallback to 32-bit if only __wasm__ is defined (legacy)
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_WASM32
#elif defined(__arm64ec__) || defined(_M_ARM64EC)
    // Arm64EC: ARM64 instructions, x64-compatible ABI. Also defines __x86_64__/_M_X64, so test it first.
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_ARM64
#elif defined(__x86_64__) || defined(_M_X64)
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_X64
#elif defined(__i386__) || defined(__i386) || defined(_M_IX86)
    // __i386 (no trailing __) is the GCC spelling; _M_IX86 is MSVC
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_IA32
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_ARM64
#elif defined(__arm__) || defined(_M_ARM)
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_ARM32
#elif defined(__riscv)
    // __riscv_xlen is always defined alongside __riscv
    #if __riscv_xlen == 64
        #define RAWR_DETECTED_ARCHID RAWR_ARCHID_RISCV64
    #else
        #define RAWR_DETECTED_ARCHID RAWR_ARCHID_RISCV32
    #endif
#elif defined(__XTENSA__)
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_XTENSA
#elif defined(__AVR__)
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_AVR
#elif defined(__powerpc__) || defined(__ppc__) || defined(_M_PPC)
    // Check for 64-bit PowerPC (includes ELFv1, ELFv2, and MSVC definitions)
    #if defined(__powerpc64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
        #define RAWR_DETECTED_ARCHID RAWR_ARCHID_PPC64
    #else
        #define RAWR_DETECTED_ARCHID RAWR_ARCHID_PPC32
    #endif
#elif defined(__loongarch64) || (defined(__loongarch__) && __loongarch_grlen == 64)
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_LOONG64
#elif defined(__mips__) || defined(_M_MRX000)
    // __mips64 or _MIPS_SIM / __mips register width checks
    #if defined(__mips64) || (defined(_MIPS_SIM) && _MIPS_SIM == _ABI64) || (defined(__mips_regsize) && __mips_regsize == 64)
        #define RAWR_DETECTED_ARCHID RAWR_ARCHID_MIPS64
    #else
        #define RAWR_DETECTED_ARCHID RAWR_ARCHID_MIPS32
    #endif
#elif defined(__s390x__) || defined(__s390__)
    // s390x is the 64-bit architecture; s390 is the legacy 32-bit
    #if defined(__s390x__)
        #define RAWR_DETECTED_ARCHID RAWR_ARCHID_S390X
    #else
        #define RAWR_DETECTED_ARCHID RAWR_ARCHID_S390
    #endif
#elif defined(__MSP430__)
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_MSP430
#elif defined(__sparc__) || defined(__sparc)
    // LEON processors (space-grade SPARC V8) are covered under __sparc__.
    // __sparcv9 and __arch64__ dictate 64-bit.
    #if defined(__sparcv9) || defined(__sparc_v9__) || defined(__arch64__)
        #define RAWR_DETECTED_ARCHID RAWR_ARCHID_SPARC64
    #else
        #define RAWR_DETECTED_ARCHID RAWR_ARCHID_SPARC32
    #endif
#elif defined(__sh__)
    // Covers SH-1 through SH-4 (SuperH)
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_SUPERH
#else
    #define RAWR_DETECTED_ARCHID RAWR_ARCHID_UNKNOWN
#endif

// --- User override. ---

// -DRAWR_SET_ARCHID=RAWR_ARCHID_<VALUE> replaces detection (UNKNOWN is a legal value). RAWR_DETECTED_ARCHID always keeps what
// detection found. Id 0 is reserved as "malformed": a misspelled name evaluates to 0.
#ifdef RAWR_ARCHID
    #error "rawr: RAWR_ARCHID is an output of detection; to override it define RAWR_SET_ARCHID"
#endif
#ifdef RAWR_SET_ARCHID
    #if RAWR_SET_ARCHID < 1 || RAWR_SET_ARCHID > RAWR_ARCHID_MAX
        #error "rawr: RAWR_SET_ARCHID must be one of the RAWR_ARCHID_* values (a misspelled name evaluates to 0, which is never valid)"
    #endif
    #define RAWR_ARCHID           RAWR_SET_ARCHID
    #define RAWR_ARCHID_DETECTED  0
    #define RAWR_ARCHID_OVERRIDEN 1
#else
    #define RAWR_ARCHID           RAWR_DETECTED_ARCHID
    #define RAWR_ARCHID_DETECTED  1
    #define RAWR_ARCHID_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#define RAWR_ARCH_IA32    0
#define RAWR_ARCH_X64     0
#define RAWR_ARCH_ARM64   0
#define RAWR_ARCH_ARM32   0
#define RAWR_ARCH_RISCV64 0
#define RAWR_ARCH_RISCV32 0
#define RAWR_ARCH_XTENSA  0
#define RAWR_ARCH_AVR     0
#define RAWR_ARCH_WASM32  0
#define RAWR_ARCH_WASM64  0
#define RAWR_ARCH_PPC32   0
#define RAWR_ARCH_PPC64   0
#define RAWR_ARCH_LOONG64 0
#define RAWR_ARCH_MIPS32  0
#define RAWR_ARCH_MIPS64  0
#define RAWR_ARCH_S390    0
#define RAWR_ARCH_S390X   0
#define RAWR_ARCH_MSP430  0
#define RAWR_ARCH_SPARC32 0
#define RAWR_ARCH_SPARC64 0
#define RAWR_ARCH_SUPERH  0
#define RAWR_ARCH_UNKNOWN 0
#if RAWR_ARCHID == RAWR_ARCHID_UNKNOWN
    #undef  RAWR_ARCH_UNKNOWN
    #define RAWR_ARCH_UNKNOWN 1
#elif RAWR_ARCHID == RAWR_ARCHID_IA32
    #undef  RAWR_ARCH_IA32
    #define RAWR_ARCH_IA32 1
#elif RAWR_ARCHID == RAWR_ARCHID_X64
    #undef  RAWR_ARCH_X64
    #define RAWR_ARCH_X64 1
#elif RAWR_ARCHID == RAWR_ARCHID_ARM64
    #undef  RAWR_ARCH_ARM64
    #define RAWR_ARCH_ARM64 1
#elif RAWR_ARCHID == RAWR_ARCHID_ARM32
    #undef  RAWR_ARCH_ARM32
    #define RAWR_ARCH_ARM32 1
#elif RAWR_ARCHID == RAWR_ARCHID_RISCV64
    #undef  RAWR_ARCH_RISCV64
    #define RAWR_ARCH_RISCV64 1
#elif RAWR_ARCHID == RAWR_ARCHID_RISCV32
    #undef  RAWR_ARCH_RISCV32
    #define RAWR_ARCH_RISCV32 1
#elif RAWR_ARCHID == RAWR_ARCHID_XTENSA
    #undef  RAWR_ARCH_XTENSA
    #define RAWR_ARCH_XTENSA 1
#elif RAWR_ARCHID == RAWR_ARCHID_AVR
    #undef  RAWR_ARCH_AVR
    #define RAWR_ARCH_AVR 1
#elif RAWR_ARCHID == RAWR_ARCHID_WASM32
    #undef  RAWR_ARCH_WASM32
    #define RAWR_ARCH_WASM32 1
#elif RAWR_ARCHID == RAWR_ARCHID_WASM64
    #undef  RAWR_ARCH_WASM64
    #define RAWR_ARCH_WASM64 1
#elif RAWR_ARCHID == RAWR_ARCHID_PPC32
    #undef  RAWR_ARCH_PPC32
    #define RAWR_ARCH_PPC32 1
#elif RAWR_ARCHID == RAWR_ARCHID_PPC64
    #undef  RAWR_ARCH_PPC64
    #define RAWR_ARCH_PPC64 1
#elif RAWR_ARCHID == RAWR_ARCHID_LOONG64
    #undef  RAWR_ARCH_LOONG64
    #define RAWR_ARCH_LOONG64 1
#elif RAWR_ARCHID == RAWR_ARCHID_MIPS32
    #undef  RAWR_ARCH_MIPS32
    #define RAWR_ARCH_MIPS32 1
#elif RAWR_ARCHID == RAWR_ARCHID_MIPS64
    #undef  RAWR_ARCH_MIPS64
    #define RAWR_ARCH_MIPS64 1
#elif RAWR_ARCHID == RAWR_ARCHID_S390
    #undef  RAWR_ARCH_S390
    #define RAWR_ARCH_S390 1
#elif RAWR_ARCHID == RAWR_ARCHID_S390X
    #undef  RAWR_ARCH_S390X
    #define RAWR_ARCH_S390X 1
#elif RAWR_ARCHID == RAWR_ARCHID_MSP430
    #undef  RAWR_ARCH_MSP430
    #define RAWR_ARCH_MSP430 1
#elif RAWR_ARCHID == RAWR_ARCHID_SPARC32
    #undef  RAWR_ARCH_SPARC32
    #define RAWR_ARCH_SPARC32 1
#elif RAWR_ARCHID == RAWR_ARCHID_SPARC64
    #undef  RAWR_ARCH_SPARC64
    #define RAWR_ARCH_SPARC64 1
#elif RAWR_ARCHID == RAWR_ARCHID_SUPERH
    #undef  RAWR_ARCH_SUPERH
    #define RAWR_ARCH_SUPERH 1
#endif

// --- Families. ---

#define RAWR_ARCH_X86   0
#define RAWR_ARCH_RISCV 0
#define RAWR_ARCH_WASM  0
#define RAWR_ARCH_ARM   0
#define RAWR_ARCH_PPC   0
#define RAWR_ARCH_MIPS  0
#define RAWR_ARCH_IBMZ  0
#define RAWR_ARCH_SPARC 0
#if RAWR_ARCH_RISCV64 || RAWR_ARCH_RISCV32
    #undef  RAWR_ARCH_RISCV
    #define RAWR_ARCH_RISCV 1
#endif
#if RAWR_ARCH_WASM32 || RAWR_ARCH_WASM64
    #undef  RAWR_ARCH_WASM
    #define RAWR_ARCH_WASM 1
#endif
#if RAWR_ARCH_X64 || RAWR_ARCH_IA32
    #undef  RAWR_ARCH_X86
    #define RAWR_ARCH_X86 1
#endif
#if RAWR_ARCH_ARM64 || RAWR_ARCH_ARM32
    #undef  RAWR_ARCH_ARM
    #define RAWR_ARCH_ARM 1
#endif
#if RAWR_ARCH_PPC64 || RAWR_ARCH_PPC32
    #undef  RAWR_ARCH_PPC
    #define RAWR_ARCH_PPC 1
#endif
#if RAWR_ARCH_MIPS64 || RAWR_ARCH_MIPS32
    #undef  RAWR_ARCH_MIPS
    #define RAWR_ARCH_MIPS 1
#endif
#if RAWR_ARCH_S390X || RAWR_ARCH_S390
    #undef  RAWR_ARCH_IBMZ
    #define RAWR_ARCH_IBMZ 1
#endif
#if RAWR_ARCH_SPARC64 || RAWR_ARCH_SPARC32
    #undef  RAWR_ARCH_SPARC
    #define RAWR_ARCH_SPARC 1
#endif
