#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.san.attributes.pp
#endif

#include "rawr/lib/compiler.pp"

#define RAWR_NO_SANITIZE_THREAD    RAWR_ATTRIBUTE(no_sanitize("thread"))
#define RAWR_NO_SANITIZE_MEMORY    RAWR_ATTRIBUTE(no_sanitize("memory"))
#define RAWR_NO_SANITIZE_UNDEFINED RAWR_ATTRIBUTE(no_sanitize("undefined"))
#define RAWR_NO_SANITIZE_HWADDRESS RAWR_ATTRIBUTE(no_sanitize("hwaddress"))
#define RAWR_NO_SANITIZE_ADDRESS   RAWR_ATTRIBUTE(no_sanitize("address"))  RAWR_DECLSPEC(no_sanitize_address)
#define RAWR_NO_SANITIZE_CFI       RAWR_ATTRIBUTE(no_sanitize("cfi"))      RAWR_DECLSPEC(guard(nocf))
