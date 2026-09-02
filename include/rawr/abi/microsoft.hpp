//// rawr/abi/microsoft.hpp.
// TODO: needs a onceover.
// TODO: requires lib::ptr<type, size> for cross-compiling things.

#ifdef RAWR_MODULE
    export module rawr.abi.microsoft;
    import rawr.lib.integer.raw;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/raw.hpp"

    #include "rawr/lib/dist/header.pp"
#endif

RAWR_EXPORT namespace rawr::abi::windows
{
    // 0x10, 0x60, etc are magic numbers, are they mandated by microsoft?

    struct unicode_string {
        ru16      length;
        ru16      max_length;
        ru16*     buffer;
    };

    struct rtl_user_process_params {
        ru8       _pad0[0x60];
        unicode_string command_line;
        unicode_string image_path;
        ru8       _pad1[0x10];
        void*    environment;
    };

    // List entry was not defined, what is it?
    struct list_entry {};
    struct ldr_data_table_entry {
        // InMemoryOrderLinks is at +0x10, not +0x00 — critical offset
        ru8                  _pad0[0x10];
        list_entry          in_memory_order_links;
        void*               dll_base;
        void*               entry_point;
        ru32                 size_of_image;
        unicode_string      full_dll_name;
        unicode_string      base_dll_name;
    };

    struct ldr_data {
        ru8          _pad0[0x10];
        list_entry  in_load_order_module_list;
        list_entry  in_memory_order_module_list;
    };

    struct peb_t {
        ru8                           _pad0[0x02];
        ru8                           being_debugged;
        ru8                           _pad1[0x05];
        void*                        mutant;
        void*                        image_base;
        ldr_data*                    ldr;
        rtl_user_process_params*     process_params;
    };
    constexpr ru8 peb_address = 0x60;
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
