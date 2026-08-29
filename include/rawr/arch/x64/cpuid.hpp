//// rawr/arch/x64/cpuid.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    import rawr.lib.bitfield;
    import rawr.lib.integer.base;
    import rawr.lib.integer.raw;
    import rawr.lib.bits;
#else
    #include "rawr/lib/bitfield.hpp"
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/bits.hpp"
#endif
#include "rawr/lib/detection.pp"
#include "rawr/lib/attributes.pp"
#include "rawr/lib/bitfield.pp"

#if RAWR_COMPILER_MSVC
    namespace rawr::arch::x64::msvc
    {
        extern "C" void __cpuidex(int cpuInfo[4], int function_id, int subfunction_id);
        #pragma intrinsic(__cpuidex)
    }
#endif

RAWR_EXPORT namespace rawr::arch::x64
{
    // TODO: this needs review for potential accidental overhead.
    struct cpuid_args { ru32 leaf, subleaf; };
    struct cpuid_ret { ru32 regs[4]; };
    RAWR_ALWAYS_INLINE auto cpuid(cpuid_args args) -> cpuid_ret
    {
        cpuid_ret ret;

        #if RAWR_COMPILER_MSVC
            msvc_detail::__cpuidex(reinterpret_cast<int*>(ret.regs), static_cast<int>(leaf), static_cast<int>(subleaf));
        #elif RAWR_COMPILER_FAMILY_GNU
            // TODO: use __builtin_ia32_cpuidext instead.
            asm volatile (
                "cpuid"
                : "=a"(ret.regs[0]), "=b"(ret.regs[1]), "=c"(ret.regs[2]), "=d"(ret.regs[3])
                : "a"(args.leaf), "c"(args.subleaf)
            );
        #else
            static_assert(false);
        #endif

        return ret;
    }

    // CPUID Leaf 1 (EAX = 1)
    // Storage layout maps exactly to registers: EAX [0-31], EBX [32-63], ECX [64-95], EDX [96-127]
    RAWR_BITFIELD_W(cpuid_leaf_1, byw16, ru32, (
        // --- EAX (Word 0: bits 0-31) ---
        (stepping,             0, 4),
        (model,                4, 4),
        (family,               8, 4),
        (processor_type,      12, 2),
        // Bits 14-15 reserved
        (extended_model,      16, 4),
        (extended_family,     20, 8),
        // Bits 28-31 reserved

        // --- EBX (Word 1: bits 32-63) ---
        (brand_index,         32, 8),
        (clflush_line_size,   40, 8),
        (max_apic_ids,        48, 8),
        (initial_apic_id,     56, 8),

        // --- ECX (Word 2: bits 64-95) ---
        (sse3,                64, 1, bool),
        (pclmulqdq,           65, 1, bool),
        (dtes64,              66, 1, bool),
        (monitor,             67, 1, bool),
        (ds_cpl,              68, 1, bool),
        (vmx,                 69, 1, bool),
        (smx,                 70, 1, bool),
        (eist,                71, 1, bool),
        (tm2,                 72, 1, bool),
        (ssse3,               73, 1, bool),
        (cnxt_id,             74, 1, bool),
        (sdbg,                75, 1, bool),
        (fma,                 76, 1, bool),
        (cmpxchg16b,          77, 1, bool),
        (xtpr_update_control, 78, 1, bool),
        (pdcm,                79, 1, bool),
        // Bit 16 (80) reserved
        (pcid,                81, 1, bool),
        (dca,                 82, 1, bool),
        (sse4_1,              83, 1, bool),
        (sse4_2,              84, 1, bool),
        (x2apic,              85, 1, bool),
        (movbe,               86, 1, bool),
        (popcnt,              87, 1, bool),
        (tsc_deadline,        88, 1, bool),
        (aesni,               89, 1, bool),
        (xsave,               90, 1, bool),
        (osxsave,             91, 1, bool),
        (avx,                 92, 1, bool),
        (f16c,                93, 1, bool),
        (rdrand,              94, 1, bool),
        (hypervisor,          95, 1, bool),

        // --- EDX (Word 3: bits 96-127) ---
        (fpu,                 96, 1, bool),
        (vme,                 97, 1, bool),
        (de,                  98, 1, bool),
        (pse,                 99, 1, bool),
        (tsc,                100, 1, bool),
        (msr,                101, 1, bool),
        (pae,                102, 1, bool),
        (mce,                103, 1, bool),
        (cx8,                104, 1, bool),
        (apic,               105, 1, bool),
        // Bit 10 (106) reserved
        (sep,                107, 1, bool),
        (mtrr,               108, 1, bool),
        (pge,                109, 1, bool),
        (mca,                110, 1, bool),
        (cmov,               111, 1, bool),
        (pat,                112, 1, bool),
        (pse_36,             113, 1, bool),
        (psn,                114, 1, bool),
        (clfsh,              115, 1, bool),
        // Bit 20 (116) reserved
        (ds,                 117, 1, bool),
        (acpi,               118, 1, bool),
        (mmx,                119, 1, bool),
        (fxsr,               120, 1, bool),
        (sse,                121, 1, bool),
        (sse2,               122, 1, bool),
        (ss,                 123, 1, bool),
        (htt,                124, 1, bool),
        (tm,                 125, 1, bool),
        // Bit 30 (126) reserved
        (pbe,                127, 1, bool)
    ), (
        [[nodiscard]] constexpr auto eax() { return storage[0]; }
        [[nodiscard]] constexpr auto ebx() { return storage[1]; }
        [[nodiscard]] constexpr auto ecx() { return storage[2]; }
        [[nodiscard]] constexpr auto edx() { return storage[3]; }
    ));

    // CPUID Leaf 7, Sub-leaf 0 (EAX = 7, ECX = 0)
    // Storage layout maps exactly to registers: EAX [0-31], EBX [32-63], ECX [64-95], EDX [96-127]
    RAWR_BITFIELD_W(cpuid_leaf_7_0, byw16, ru32,
    (
        // --- EAX (Word 0: bits 0-31) ---
        (max_sub_leaf,         0, 32), // Reports the maximum supported sub-leaf of leaf 7

        // --- EBX (Word 1: bits 32-63) ---
        (fsgsbase,            32, 1, bool),
        (ia32_tsc_adjust,     33, 1, bool),
        (sgx,                 34, 1, bool),
        (bmi1,                35, 1, bool),
        (hle,                 36, 1, bool),
        (avx2,                37, 1, bool),
        (fdp_excptn_only,     38, 1, bool),
        (smep,                39, 1, bool),
        (bmi2,                40, 1, bool),
        (erms,                41, 1, bool),
        (invpcid,             42, 1, bool),
        (rtm,                 43, 1, bool),
        (rdt_m,               44, 1, bool),
        (deprecated_fpu,      45, 1, bool), // FPU CS/DS deprecated
        (mpx,                 46, 1, bool),
        (pqe,                 47, 1, bool),
        (avx512f,             48, 1, bool),
        (avx512dq,            49, 1, bool),
        (rdseed,              50, 1, bool),
        (adx,                 51, 1, bool),
        (smap,                52, 1, bool),
        (avx512_ifma,         53, 1, bool),
        (pcommit,             54, 1, bool),
        (clflushopt,          55, 1, bool),
        (clwb,                56, 1, bool),
        (intel_pt,            57, 1, bool),
        (avx512pf,            58, 1, bool),
        (avx512er,            59, 1, bool),
        (avx512cd,            60, 1, bool),
        (sha,                 61, 1, bool),
        (avx512bw,            62, 1, bool),
        (avx512vl,            63, 1, bool),

        // --- ECX (Word 2: bits 64-95) ---
        (prefetchwt1,         64, 1, bool),
        (avx512_vbmi,         65, 1, bool),
        (umip,                66, 1, bool),
        (pku,                 67, 1, bool),
        (ospke,               68, 1, bool),
        (waitpkg,             69, 1, bool),
        (avx512_vbmi2,        70, 1, bool),
        (cet_ss,              71, 1, bool),
        (gfni,                72, 1, bool),
        (vaes,                73, 1, bool),
        (vpclmulqdq,          74, 1, bool),
        (avx512_vnni,         75, 1, bool),
        (avx512_bitalg,       76, 1, bool),
        (tme_en,              77, 1, bool),
        (avx512_vpopcntdq,    78, 1, bool),
        // Bit 15 (79) reserved
        (la57,                80, 1, bool),
        (mawau,               81, 5), // Bits 17-21
        (rdpid,               86, 1, bool), // Bit 22
        (kl,                  87, 1, bool),
        (bus_lock_detect,     88, 1, bool),
        (cldemote,            89, 1, bool),
        // Bit 26 (90) reserved
        (movdiri,             91, 1, bool),
        (movdir64b,           92, 1, bool),
        (enqcmd,              93, 1, bool),
        (sgx_lc,              94, 1, bool),
        (pks,                 95, 1, bool),

        // --- EDX (Word 3: bits 96-127) ---
        // Bits 0-1 (96-97) reserved
        (avx512_4vnniw,       98, 1, bool),
        (avx512_4fmaps,       99, 1, bool),
        (fsrm,               100, 1, bool),
        (uintr,              101, 1, bool),
        // Bits 6-7 (102-103) reserved
        (avx512_vp2intersect,104, 1, bool),
        (srbds_ctrl,         105, 1, bool),
        (md_clear,           106, 1, bool),
        (rtm_always_abort,   107, 1, bool),
        // Bit 12 (108) reserved
        (tsx_force_abort,    109, 1, bool),
        (serialize,          110, 1, bool),
        (hybrid,             111, 1, bool),
        (tsxldtrk,           112, 1, bool),
        // Bit 17 (113) reserved
        (pconfig,            114, 1, bool),
        (lbr,                115, 1, bool),
        (cet_ibt,            116, 1, bool),
        // Bit 21 (117) reserved
        (amx_bf16,           118, 1, bool),
        (avx512_fp16,        119, 1, bool),
        (amx_tile,           120, 1, bool),
        (amx_int8,           121, 1, bool),
        (ibrs_ibpb,          122, 1, bool),
        (stibp,              123, 1, bool),
        (l1d_flush,          124, 1, bool),
        (ia32_arch_cap,      125, 1, bool),
        (ia32_core_cap,      126, 1, bool),
        (ssbd,               127, 1, bool)
    ), (
        [[nodiscard]] constexpr auto eax() { return storage[0]; }
        [[nodiscard]] constexpr auto ebx() { return storage[1]; }
        [[nodiscard]] constexpr auto ecx() { return storage[2]; }
        [[nodiscard]] constexpr auto edx() { return storage[3]; }
    ));
}
