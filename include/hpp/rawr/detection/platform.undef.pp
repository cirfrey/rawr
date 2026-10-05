// GENERATED — do not edit
#if defined(__EMSCRIPTEN__) || defined(__wasm__)
    #undef RAWR_PLATFORM_WASM
#elif defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP32)
    #undef RAWR_PLATFORM_ESP32
#elif defined(ESP8266) || defined(ARDUINO_ARCH_ESP8266)
    #undef RAWR_PLATFORM_ESP8266
#elif defined(__APPLE__)
    #if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
        #undef RAWR_PLATFORM_MACOS
    #elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)       || defined(__ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__)
        #undef RAWR_PLATFORM_IOS
    #else
        #undef RAWR_PLATFORM_UNKNOWN
    #endif
#elif defined(__ANDROID__)
    #undef RAWR_PLATFORM_ANDROID
#elif defined(__linux__)
    #undef RAWR_PLATFORM_LINUX
#elif defined(_WIN32) || defined(_WIN64)
    #undef RAWR_PLATFORM_WINDOWS
#elif defined(USE_HAL_DRIVER)           || defined(STM32F0) || defined(STM32F1) || defined(STM32F2) || defined(STM32F3) || defined(STM32F4) || defined(STM32F7) || defined(STM32G0) || defined(STM32G4) || defined(STM32H7) || defined(STM32L0) || defined(STM32L1) || defined(STM32L4) || defined(STM32L5) || defined(STM32U5) || defined(STM32WB) || defined(STM32WL)
    #undef RAWR_PLATFORM_STM32
#elif defined(NRF51) || defined(NRF52) || defined(NRF9160)
    #undef RAWR_PLATFORM_NORDIC
#elif defined(PICO_BOARD) || defined(RASPBERRYPI_PICO)
    #undef RAWR_PLATFORM_PICO
#elif defined(TEENSYDUINO) || defined(__IMXRT1062__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
    #undef RAWR_PLATFORM_TEENSY
#elif defined(__AVR__)
    #undef RAWR_PLATFORM_AVR
#elif defined(__FreeBSD__)
    #undef RAWR_PLATFORM_FREEBSD
#elif defined(__OpenBSD__)
    #undef RAWR_PLATFORM_OPENBSD
#elif defined(__NetBSD__)
    #undef RAWR_PLATFORM_NETBSD
#else
    #undef RAWR_PLATFORM_UNKNOWN
#endif
#undef RAWR_PLATFORM_UNKNOWN
#undef RAWR_PLATFORM_AVR
#undef RAWR_PLATFORM_TEENSY
#undef RAWR_PLATFORM_PICO
#undef RAWR_PLATFORM_NORDIC
#undef RAWR_PLATFORM_STM32
#undef RAWR_PLATFORM_ESP8266
#undef RAWR_PLATFORM_ESP32
#undef RAWR_PLATFORM_WASM
#undef RAWR_PLATFORM_ANDROID
#undef RAWR_PLATFORM_IOS
#undef RAWR_PLATFORM_MACOS
#undef RAWR_PLATFORM_NETBSD
#undef RAWR_PLATFORM_OPENBSD
#undef RAWR_PLATFORM_FREEBSD
#undef RAWR_PLATFORM_WINDOWS
#undef RAWR_PLATFORM_LINUX
#include "rawr/distribution/todo.undef.pp"
#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
#endif
