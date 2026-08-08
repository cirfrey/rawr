#pragma region rawr/abi/microsoft.hpp

#ifdef RAWR_UNITY
    #pragma once
    #include "rawr/data/aliases.hpp"
    #include "rawr/cxx/intrin.hpp"
#endif

// TODO: implement lib::ptr<type, size> for cross-compiling things.
namespace rawr::abi::windows
{

    struct unicode_string {
        u16      length;
        u16      max_length;
        u16*     buffer;
    };

    struct rtl_user_process_params {
        u8       _pad0[0x60];
        unicode_string command_line;
        unicode_string image_path;
        u8       _pad1[0x10];
        void*    environment;
    };

    // List entry was not defined, what is it?
    struct list_entry {};
    struct ldr_data_table_entry {
        // InMemoryOrderLinks is at +0x10, not +0x00 — critical offset
        u8                  _pad0[0x10];
        list_entry          in_memory_order_links;
        void*               dll_base;
        void*               entry_point;
        u32                 size_of_image;
        unicode_string      full_dll_name;
        unicode_string      base_dll_name;
    };

    struct ldr_data {
        u8          _pad0[0x10];
        list_entry  in_load_order_module_list;
        list_entry  in_memory_order_module_list;
    };

    struct peb_t {
        u8                           _pad0[0x02];
        u8                           being_debugged;
        u8                           _pad1[0x05];
        void*                        mutant;
        void*                        image_base;
        ldr_data*                    ldr;
        rtl_user_process_params*     process_params;
    };
    constexpr u8 peb_address = 0x60;
    // While you can use this on any platform, it only really
    // makes *sense* to do it on windows. On linux for instance you'll
    // write/read into the zero page and recieve a nice SIGEGV.
    //
    // Should we guard this in a #if RAWR_PLATFORM_WINDOWS? Should we
    // be patronizing and tell what the user can and can't do?
    // I say nay! Open pandora's box and let the consequences (and perhaps
    // bounties that await) find us!
    //
    // NOTE: What could possibly be the use of this outside windows?!
    inline peb_t& peb = *reinterpret_cast<peb_t*>(peb_address);
}

#define RAWR_ABI_MICROSOFTX64_MAIN(...)                                \
    _Pragma("comment(linker, \"/entry:_start\")")                      \
    template<typename Ctx>                                             \
    RAWR_ALWAYS_INLINE RAWR_NORETURN void rawr_main(Ctx& ctx) noexcept \
    { __VA_ARGS__ }                                                    \
    RAWR_NORETURN void _start() noexcept                               \
    {                                                                  \
        auto ctx = 0;                                                  \
        rawr_main(ctx);                                                \
    }                                                                  \
