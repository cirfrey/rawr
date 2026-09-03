//// rawr/platform/linux.hpp.

#ifdef RAWR_MODULE
    export module rawr.platform.linux;
    import rawr.lib.integer.base;
    import rawr.lib.integer.raw;
    import rawr.lib.detection;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/detection.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/attributes.pp"
#include "rawr/lib/detection.pp"
#include "rawr/lib/pp.pp"
#include "rawr/lib/rich_enum.pp"

RAWR_EXPORT namespace rawr::platform::linux
{
    // Not named errno to avoid conflict with platform header macros.
    RAWR_RICH_ENUM(err, ru16, (
        (success, 0), // No error.

        // General / Permissions.
        (eperm,   1),  // Operation not permitted.
        (enoent,  2),  // No such file or directory.
        (esrch,   3),  // No such process.
        (eintr,   4),  // Interrupted system call.
        (eio,     5),  // I/O error.
        (enxio,   6),  // No such device or address.
        (e2big,   7),  // Argument list too long.
        (enoexec, 8),  // Exec format error.
        (ebadf,   9),  // Bad file number.
        (echild,  10), // No child processes.
        (eagain,  11), // Try again (also EWOULDBLOCK).
        (enomem,  12), // Out of memory.
        (eacces,  13), // Permission denied.
        (efault,  14), // Bad address.

        // Filesystem / Block Devices.
        (enotblk, 15), // Block device required.
        (ebusy,   16), // Device or resource busy.
        (eexist,  17), // File exists.
        (exdev,   18), // Cross-device link.
        (enodev,  19), // No such device.
        (enotdir, 20), // Not a directory.
        (eisdir,  21), // Is a directory.
        (einval,  22), // Invalid argument.
        (enfile,  23), // File table overflow.
        (emfile,  24), // Too many open files.
        (enotty,  25), // Not a typewriter (ioctl error).
        (etxtbsy, 26), // Text file busy.
        (efbig,   27), // File too large.
        (enospc,  28), // No space left on device.
        (espipe,  29), // Illegal seek.
        (erofs,   30), // Read-only file system.
        (emlink,  31), // Too many links.
        (epipe,   32), // Broken pipe.

        // Math.
        (edom,    33), // Math argument out of domain.
        (erange,  34), // Math result not representable.

        // Extended System & IPC (35 - 87).
        // These handle symbolic links, deadlocks, and IPC (Inter-Process Communication).
        (edeadlk,      35), // Resource deadlock would occur.
        (enametoolong, 36), // File name too long.
        (enolck,       37), // No record locks available.
        (enosys,       38), // Invalid system call number.
        (enotempty,    39), // Directory not empty.
        (eloop,        40), // Too many symbolic links encountered.
        (enomsg,       42), // No message of desired type.
        (eidrm,        43), // Identifier removed.
        (enodata,      61), // No data available.
        (etime,        62), // Timer expired.
        (eproto,       71), // Protocol error.
        (eoverflow,    75), // Value too large for defined data type.
        (ebadfd,       77), // File descriptor in bad state.

        // Network & Sockets.
        (enotsock,        88),  // Socket operation on non-socket.
        (edestaddrreq,    89),  // Destination address required.
        (emsgsize,        90),  // Message too long.
        (eprototype,      91),  // Protocol wrong type for socket.
        (enoprotoopt,     92),  // Protocol not available.
        (eprotonosupport, 93),  // Protocol not supported.
        (esocktnosupport, 94),  // Socket type not supported.
        (enotsup,         95),  // Operation not supported (also EOPNOTSUPP).
        (epfnosupport,    96),  // Protocol family not supported.
        (eafnosupport,    97),  // Address family not supported by protocol.
        (eaddrinuse,      98),  // Address already in use.
        (eaddrnotavail,   99),  // Cannot assign requested address.
        (enetdown,        100), // Network is down.
        (enetunreach,     101), // Network is unreachable.
        (enetreset,       102), // Network dropped connection on reset.
        (econnaborted,    103), // Software caused connection abort.
        (econnreset,      104), // Connection reset by peer.
        (enobufs,         105), // No buffer space available.
        (eisconn,         106), // Transport endpoint is already connected.
        (enotconn,        107), // Transport endpoint is not connected.
        (eshutdown,       108), // Cannot send after transport endpoint shutdown.
        (etoomanyrefs,    109), // Too many references: cannot splice.
        (etimedout,       110), // Connection timed out.
        (econnrefused,    111), // Connection refused.
        (ehostdown,       112), // Host is down.
        (ehostunreach,    113), // No route to host.
        (ealready,        114), // Operation already in progress.
        (einprogress,     115), // Operation now in progress.

        // Modern linux stuff.
        (estale,          116), // Stale file handle (NFS).
        (edquot,          122), // Quota exceeded.
        (ecanceled,       125), // Operation Canceled (AIO).
        (eownerdead,      130), // Owner died (Robust Futexes).
        (enotrecoverable, 131), // State not recoverable (Robust Futexes).
        (erfkill,         132), // Operation not possible due to RF-kill.
        (ehwpoison,       133), // Memory page has hardware error.

        (max,             4095) // Not actually observed, just the max value the linux kerned could possibly return.
    ), ());

    // Doesn't correspond to the actual number, just an id.
    // NOLINTBEGIN(performance-enum-size)
    enum class syscall_id : ru16
    {
        write,
        exit
    };
    // NOLINTEND(performance-enum-size)
    struct syscall_metadata {
        syscall_id id;
        ru16       number;
        ru8        argcount;
        archs      arch;
    };

    namespace x64 {
        using reg_t = rs64;
        using fd_t  = rs32;

        constexpr fd_t stdin  = 0;
        constexpr fd_t stdout = 1;
        constexpr fd_t stderr = 2;

        constexpr char const* syscall_register = "rax";
        constexpr char const* return_register  = "rax";
        constexpr char const* arg_registers[]  = {"rdi", "rsi", "rdx", "r10", "r8", "r9"};

        namespace metadata
        {
            constexpr syscall_metadata write = { .id = syscall_id::write, .number = 1,  .argcount = 3, .arch = archs::x64 };
            constexpr syscall_metadata exit  = { .id = syscall_id::exit,  .number = 60, .argcount = 1, .arch = archs::x64 };
        }
    }
}

// ── RAWR_PLATFORM_LINUX_SYSCALL_RETURN ────────────────────────────────────────
// Produces name_error (RAWR_RICH_ENUM) and name_r ([[nodiscard]] result struct)
// as a unified pair from per-syscall error declarations.
//
// Entry format: (errno_alias, semantic_name)
//   errno_alias   — rawr::platform::linux::err enumerator (e.g. eintr)
//   semantic_name — descriptive name for call sites (e.g. interrupted)
//   Value derived: rawr::platform::linux::err::alias.to_underlying()
//   Both names receive is_* and on_* on name_r.
//
// Calling convention:
//   RAWR_PLATFORM_LINUX_SYSCALL_RETURN(
//       Name,
//       RegT,               // register width type, should be signed (s32 / s64).
//       RegName,            // Name for the register member inside the struct.
//       OkT,                // success value type   (integer, void*, void).
//       EPairs,             // ((errno_alias, semantic_name), (c, d), ...) pairs, comma-separated.
//       (as_ok return),     // return of as_ok().
//       (is_error return),  // return of is_error().
//       (extra methods)     // injected verbatim into name_r.
//   )
// NOLINTBEGIN(performance-enum-size)
#define RAWR_PLATFORM_LINUX_SYSCALL_RETURN(Name, RegT, RegName, OkT, EPairs, IsError, AsOk, Body)        \
    RAWR_RICH_ENUM(                                                                                      \
        Name##_error,                                                                                    \
        ::rawr::platform::linux::err::underlying_type,                                                   \
        (RAWR_PP_EACH_SEP(RAWR_PLATFORM_LINUX_SYSCALL_EPAIR, RAWR_PP_COMMA_SEP, RAWR_PP_STRIP(EPairs))), \
    (                                                                                                    \
        [[nodiscard]] constexpr auto to_errno() const noexcept {                                         \
            return ::rawr::platform::linux::err::from_underlying_unchecked(                              \
                static_cast<::rawr::platform::linux::err::underlying_type>(to_underlying()));            \
        }                                                                                                \
    ))                                                                                                   \
                                                                                                         \
    struct [[nodiscard]] Name##_r {                                                                      \
    private:                                                                                             \
        using self = Name##_r;                                                                           \
    public:                                                                                              \
        using error = Name##_error;                                                                      \
        using reg   = RegT;                                                                              \
        using ok    = OkT;                                                                               \
                                                                                                         \
        RegT RegName;                                                                                    \
                                                                                                         \
        [[nodiscard]] constexpr auto is_error() const noexcept { return RAWR_PP_STRIP(IsError); }        \
        [[nodiscard]] constexpr auto is_ok()    const noexcept { return !is_error(); }                   \
                                                                                                         \
        template<typename OkT_ = ok>                                                                     \
        [[nodiscard]] constexpr auto as_ok() const noexcept -> ok                                        \
        requires (requires { sizeof(OkT_); })                                                            \
        { return RAWR_PP_STRIP(AsOk); }                                                                  \
                                                                                                         \
        [[nodiscard]] constexpr auto as_error() const noexcept {                                         \
            return error::from_underlying_unchecked(                                                     \
                static_cast<error::underlying_type>(-RegName));                                          \
        }                                                                                                \
        [[nodiscard]] constexpr auto as_safe_error() const noexcept {                                    \
            return error::from_underlying_unchecked(                                                     \
                static_cast<error::underlying_type>(is_error() ? -RegName : 0));                         \
        }                                                                                                \
        [[nodiscard]] constexpr auto as_errno() const noexcept {                                         \
            return ::rawr::platform::linux::err::from_underlying_unchecked(                              \
                static_cast<                                                                             \
                    ::rawr::platform::linux::err::underlying_type                                        \
                >(is_error() ? -RegName : 0));                                                           \
        }                                                                                                \
                                                                                                         \
        constexpr void discard() const noexcept {}                                                       \
                                                                                                         \
        RAWR_PP_EACH(RAWR_PLATFORM_LINUX_SYSCALL_IS, RAWR_PP_STRIP(EPairs))                              \
        RAWR_PP_EACH(RAWR_PLATFORM_LINUX_SYSCALL_ON, RAWR_PP_STRIP(EPairs))                              \
                                                                                                         \
        RAWR_PP_STRIP(Body)                                                                              \
    };
// NOLINTEND(performance-enum-size)
// Auxiliary macros
#define RAWR_PLATFORM_LINUX_SYSCALL_EPAIR_2(canon, sem)           \
    (canon, ::rawr::platform::linux::err::canon.to_underlying()), \
    (sem,   ::rawr::platform::linux::err::canon.to_underlying())
#define RAWR_PLATFORM_LINUX_SYSCALL_EPAIR(epair) RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_PLATFORM_LINUX_SYSCALL_EPAIR_, epair)
#define RAWR_PLATFORM_LINUX_SYSCALL_IS_2(canon, sem)                                                              \
    [[nodiscard]] constexpr auto is_##canon() const noexcept { return is_error() && as_error() == error::canon; } \
    [[nodiscard]] constexpr auto is_##sem()   const noexcept { return is_error() && as_error() == error::sem; }
#define RAWR_PLATFORM_LINUX_SYSCALL_IS(epair) RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_PLATFORM_LINUX_SYSCALL_IS_, epair)
#define RAWR_PLATFORM_LINUX_SYSCALL_ON_2(canon, sem)                                         \
    constexpr auto on_##canon(auto&& callback) noexcept -> self& { if (is_##canon()) callback(); return *this; } \
    constexpr auto on_##sem(auto&& callback)   noexcept -> self& { if (is_##sem())   callback(); return *this; }
#define RAWR_PLATFORM_LINUX_SYSCALL_ON(epair) RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_PLATFORM_LINUX_SYSCALL_ON_, epair)

RAWR_EXPORT namespace rawr::platform::linux::x64::syscall
{
    #if RAWR_PLATFORM_LINUX && RAWR_ARCH_X64 && RAWR_COMPILER_FAMILY_GNU
        #define RAWR_PLATFORM_LINUX_X64_SYSCALL_GATED_BODY(...) \
            __VA_ARGS__;
    #else
        #define RAWR_PLATFORM_LINUX_X64_SYSCALL_GATED_BODY(...) \
            static_assert(false, "Unsupported compiler for x64 syscalls, implement the override yourself.");
    #endif


    RAWR_PLATFORM_LINUX_SYSCALL_RETURN(write, reg_t, rax, ru64, (
        (success, no_error),
        (eintr,   interrupted),  // signal before any bytes written — retry full call with same args
        (ebadf,   bad_fd),       // fd not valid or not open for writing
        (efault,  bad_buffer),   // buf outside accessible address space
        (einval,  invalid),      // fd unsuitable for writing; O_DIRECT alignment unmet
        (eio,     io_error),     // low-level I/O error modifying the inode
        (enospc,  no_space),     // no space left on device
        (epipe,   broken_pipe),  // reading end of pipe or socket closed
        (eagain,  would_block),  // O_NONBLOCK set and write would block
        (edquot,  quota_exceeded)// user disk quota exhausted
    ), (rax < 0), (rax), ())
    template <compilers C = this_compiler, archs A = this_arch>
    RAWR_ALWAYS_INLINE auto write(fd_t file, char const* data, ruint auto size) -> write_r{
        reg_t rax = metadata::write.number;
        RAWR_PLATFORM_LINUX_X64_SYSCALL_GATED_BODY(
            // Microoptimization: Forces 32-bit register constraints (edx) for sizes <= 4 bytes
            // to prevent REX prefixes in instruction encoding, while zero-extending into rdx
            // for the kernel, avoiding binary bloat on small literals. Saves 1 byte of asm per syscall.
            // - 64-bit operation (rdx): Forces the compiler to emit a REX prefix byte, making the instruction 3 bytes. E.g., [mov rdx, rax] is [48 89 c2].
            // - 32-bit operation (edx): Operates on the native 32-bit legacy register space (no REX prefix - 2 bytes). E.g., [mov edx, eax] is [89 c2].
            if constexpr (sizeof(size) <= 4)
            {
                asm volatile("syscall"
                    : "+a"(rax)
                    : "D"(file), "S"(data), "d"(static_cast<ru32>(size))
                    : "rcx", "r11", "memory"
                );
            }
            else
            {
                asm volatile("syscall"
                    : "+a"(rax)
                    : "D"(file), "S"(data), "d"(static_cast<ru64>(size))
                    : "rcx", "r11", "memory"
                );
            }
        )
        return { rax };
    }

    // Explicit specialization for known sizes, better cloberring.
    // NOTE: this doesn't do any null-terminator stripping, it just outputs what it gets.
    template <rst Size, compilers C = this_compiler, archs A = this_arch>
    RAWR_ALWAYS_INLINE auto write(fd_t file, char const(&data)[Size]) -> write_r {
        reg_t rax = metadata::write.number;
        RAWR_PLATFORM_LINUX_X64_SYSCALL_GATED_BODY(
            // Same microoptimization.
            if constexpr (Size <= aint_max<ru32>)
            {
                asm volatile("syscall"
                    : "+a"(rax)
                    : "D"(file), "S"(data), "d"(static_cast<ru32>(Size)), "m"(data)
                    : "rcx", "r11"
                );
            }
            else
            {
                asm volatile("syscall"
                    : "+a"(rax)
                    : "D"(file), "S"(data), "d"(static_cast<ru64>(Size)), "m"(data)
                    : "rcx", "r11"
                );
            }
        )
        return { rax };
    }

    // Explicit specialization for chars.
    template <compilers C = this_compiler, archs A = this_arch>
    RAWR_ALWAYS_INLINE auto write(fd_t file, char data) -> write_r {
        const char arr[] = { data };
        return write<1>(file, arr);
    }

    template <compilers C = this_compiler, archs A = this_arch>
    RAWR_ALWAYS_INLINE RAWR_NORETURN auto exit(ru8 code) -> void
    {
        RAWR_PLATFORM_LINUX_X64_SYSCALL_GATED_BODY(
            asm volatile("syscall" :: "a"(metadata::exit.number), "D"(code));
            RAWR_UNREACHABLE;
        )
    }
}

#undef RAWR_PLATFORM_LINUX_SYSCALL_RETURN
#undef RAWR_PLATFORM_LINUX_SYSCALL_EPAIR_2
#undef RAWR_PLATFORM_LINUX_SYSCALL_EPAIR
#undef RAWR_PLATFORM_LINUX_SYSCALL_IS_2
#undef RAWR_PLATFORM_LINUX_SYSCALL_IS
#undef RAWR_PLATFORM_LINUX_SYSCALL_ON_2
#undef RAWR_PLATFORM_LINUX_SYSCALL_ON
#undef RAWR_PLATFORM_LINUX_X64_SYSCALL_GATED_BODY
