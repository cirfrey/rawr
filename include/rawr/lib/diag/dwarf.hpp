//// rawr/lib/diag/dwarf.hpp.

#ifdef RAWR_MODULE
    export module rawr.lib.diag.dwarf;
    import rawr.lib.detection;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/detection.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"
#include "rawr/lib/attributes.pp"

namespace rawr::inline lib::diag::dwarf
{
    enum class x64_reg : unsigned char {
        rax = 0,  rdx = 1,  rcx = 2,  rbx = 3,  rsi = 4,  rdi = 5,
        rbp = 6,  rsp = 7,  r8  = 8,  r9  = 9,  r10 = 10, r11 = 11,
        r12 = 12, r13 = 13, r14 = 14, r15 = 15,
        rip = 16, // return address (RA)
        // x87/MMX/SSE registers omitted; rarely needed for unwind.
        // TODO: un-omit them, this should be a complete representation.
    };

    enum class arm64_reg : unsigned char {
        x0  = 0,  x1  = 1,  x2  = 2,  x3  = 3,  x4  = 4,  x5  = 5,
        x6  = 6,  x7  = 7,  x8  = 8,  x9  = 9,  x10 = 10, x11 = 11,
        x12 = 12, x13 = 13, x14 = 14, x15 = 15, x16 = 16, x17 = 17,
        x18 = 18, x19 = 19, x20 = 20, x21 = 21, x22 = 22, x23 = 23,
        x24 = 24, x25 = 25, x26 = 26, x27 = 27, x28 = 28,
        x29 = 29, // frame pointer
        x30 = 30, // link register (LR) → return address
        sp  = 31, // stack pointer
    };

    enum class riscv64_reg : unsigned char {
        zero = 0,  ra   = 1,  sp   = 2,  gp   = 3,  tp   = 4,
        t0   = 5,  t1   = 6,  t2   = 7,  s0   = 8,  s1   = 9,
        a0   = 10, a1   = 11, a2   = 12, a3   = 13, a4   = 14,
        a5   = 15, a6   = 16, a7   = 17, s2   = 18, s3   = 19,
        s4   = 20, s5   = 21, s6   = 22, s7   = 23, s8   = 24,
        s9   = 25, s10  = 26, s11  = 27, t3   = 28, t4   = 29,
        t5   = 30, t6   = 31,
        // No dedicated return‑address register; RA is x1 (same as 'ra').
    };

    // CFI marker – only emitted when the toolchain supports DWARF CFI in asm
    // and exceptions are enabled (otherwise it's inert or an assembler error).
    template <rawr::archs A = this_arch>
    RAWR_ALWAYS_INLINE void mark_unwind_root()
    {
        #if RAWR_HAS_CFI_ASM && RAWR_HAS_EXCEPTIONS
                 if constexpr(A.is_x64())     { asm volatile(".cfi_undefined 16"); }
            else if constexpr(A.is_arm64())   { asm volatile(".cfi_undefined 30"); }
            else if constexpr(A.is_riscv64()) { asm volatile(".cfi_undefined 1"); }
            else                              { static_assert(false, "Unimplemented"); }
        #endif
    }
}
