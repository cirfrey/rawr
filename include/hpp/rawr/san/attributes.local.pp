// GENERATED — do not edit

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.san.attributes.pp
#endif

#include "rawr/lib/compiler.local.pp"

#undef RAWR_NO_SANITIZE_THREAD
#define RAWR_NO_SANITIZE_THREAD    RAWR_ATTRIBUTE(no_sanitize("thread"))
#undef RAWR_NO_SANITIZE_MEMORY
#define RAWR_NO_SANITIZE_MEMORY    RAWR_ATTRIBUTE(no_sanitize("memory"))
#undef RAWR_NO_SANITIZE_UNDEFINED
#define RAWR_NO_SANITIZE_UNDEFINED RAWR_ATTRIBUTE(no_sanitize("undefined"))
#undef RAWR_NO_SANITIZE_HWADDRESS
#define RAWR_NO_SANITIZE_HWADDRESS RAWR_ATTRIBUTE(no_sanitize("hwaddress"))
#undef RAWR_NO_SANITIZE_ADDRESS
#define RAWR_NO_SANITIZE_ADDRESS   RAWR_ATTRIBUTE(no_sanitize("address"))  RAWR_DECLSPEC(no_sanitize_address)
#undef RAWR_NO_SANITIZE_CFI
#define RAWR_NO_SANITIZE_CFI       RAWR_ATTRIBUTE(no_sanitize("cfi"))      RAWR_DECLSPEC(guard(nocf))
