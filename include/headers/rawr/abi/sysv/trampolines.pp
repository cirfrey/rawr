#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/abi/sysv.pp"
#endif
#pragma once

#include "rawr/lib/detection.pp"

// Most of these get a Tail‑call (jmp/b/j) – preserves kernel‑provided
// 16‑byte alignment, no stack push, no alignment fixup needed.
// Except x86-32: esp points to argc at entry. Pass original esp as void* sp argument.
// Push before align — otherwise the push itself breaks the alignment we just set.
// "and $-16, %esp" first would lose the original esp value.
#define RAWR_ABI_SYSV_TRAMPOLINE_X86 \
    "movl %esp, %eax\n\t"    /* save original sp */ \
    "andl $-16, %esp\n\t"    /* align (kernel may deliver esp-4 ≡ 0 mod 16; verify per ABI) */ \
    "pushl %eax\n\t"         /* push sp as argument */ \
    "call rawr_main"         /* must be call, not jmp — arg is on stack */
#define RAWR_ABI_SYSV_TRAMPOLINE_X64     "mov %rsp, %rdi\n\tjmp rawr_main"
#define RAWR_ABI_SYSV_TRAMPOLINE_ARM32   "mov r0, sp\n\tb rawr_main"
#define RAWR_ABI_SYSV_TRAMPOLINE_ARM64   "mov x0, sp\n\tb rawr_main"
#define RAWR_ABI_SYSV_TRAMPOLINE_RISCV32 "mv a0, sp\n\tj rawr_main"
#define RAWR_ABI_SYSV_TRAMPOLINE_RISCV64 "mv a0, sp\n\tj rawr_main"

#define RAWR_ABI_SYSV_HAS_TRAMPOLINE 1
#if RAWR_ARCH_X86
    #define RAWR_ABI_SYSV_TRAMPOLINE RAWR_ABI_SYSV_TRAMPOLINE_X86
#elif RAWR_ARCH_X64
    #define RAWR_ABI_SYSV_TRAMPOLINE RAWR_ABI_SYSV_TRAMPOLINE_X86
#elif RAWR_ARCH_ARM32
    #define RAWR_ABI_SYSV_TRAMPOLINE RAWR_ABI_SYSV_TRAMPOLINE_ARM32
#elif RAWR_ARCH_ARM64
    #define RAWR_ABI_SYSV_TRAMPOLINE RAWR_ABI_SYSV_TRAMPOLINE_ARM64
#elif RAWR_ARCH_RISCV32
    #define RAWR_ABI_SYSV_TRAMPOLINE RAWR_ABI_SYSV_TRAMPOLINE_RISCV32
#elif RAWR_ARCH_RISCV64
    #define RAWR_ABI_SYSV_TRAMPOLINE RAWR_ABI_SYSV_TRAMPOLINE_RISCV64
#else
    #define RAWR_ABI_SYSV_TRAMPOLINE ".error \"Missing trampoline\""
    #undef  RAWR_ABI_SYSV_HAS_TRAMPOLINE
    #define RAWR_ABI_SYSV_HAS_TRAMPOLINE 0
#endif
