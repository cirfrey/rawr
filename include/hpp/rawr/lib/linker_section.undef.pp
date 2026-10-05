// GENERATED — do not edit
#undef RAWR_LINKER_SECTION_REGISTER
#if RAWR_BIN_PE && RAWR_COMPILER_MSVC
    #undef RAWR_LS_DETAIL_REGISTER_
    #undef RAWR_LINKER_SECTION_DEFINE
#elif RAWR_BIN_PE
    #undef RAWR_LS_DETAIL_REGISTER_
    #undef RAWR_LINKER_SECTION_DEFINE
#elif RAWR_BIN_MACHO
    #undef RAWR_LS_DETAIL_REGISTER_
    #undef RAWR_LINKER_SECTION_DEFINE
#elif RAWR_BIN_ELF
    #undef RAWR_LS_DETAIL_REGISTER_
    #undef RAWR_LINKER_SECTION_DEFINE
#endif
#undef RAWR_LS_DETAIL_VALIDATE_NAME_
#undef RAWR_LS_STR
#undef RAWR_LS_STR_
#undef RAWR_LS_CONCAT
#undef RAWR_LS_CONCAT_
#include "rawr/detection/bin.undef.pp"
#include "rawr/detection/compiler.undef.pp"
#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
#endif
