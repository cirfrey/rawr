#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.detection.platform.pp
#endif

#include "rawr/distribution/todo.pp"

RAWR_TODO("detect wasi")

#define RAWR_PLATFORM_LINUX   0
#define RAWR_PLATFORM_WINDOWS 0
#define RAWR_PLATFORM_FREEBSD 0
#define RAWR_PLATFORM_OPENBSD 0
#define RAWR_PLATFORM_NETBSD  0
#define RAWR_PLATFORM_MACOS   0
#define RAWR_PLATFORM_IOS     0
#define RAWR_PLATFORM_ANDROID 0
#define RAWR_PLATFORM_WASM    0
#define RAWR_PLATFORM_ESP32   0
#define RAWR_PLATFORM_ESP8266 0
#define RAWR_PLATFORM_STM32   0
#define RAWR_PLATFORM_NORDIC  0
#define RAWR_PLATFORM_PICO    0
#define RAWR_PLATFORM_TEENSY  0
#define RAWR_PLATFORM_AVR     0
#define RAWR_PLATFORM_UNKNOWN 0
// WASM: both Emscripten and standalone WASM runtimes
#if defined(__EMSCRIPTEN__) || defined(__wasm__)
    #undef  RAWR_PLATFORM_WASM
    #define RAWR_PLATFORM_WASM 1
// ESP32: ESP-IDF defines ESP_PLATFORM; Arduino framework defines ARDUINO_ARCH_ESP32
#elif defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP32)
    #undef  RAWR_PLATFORM_ESP32
    #define RAWR_PLATFORM_ESP32 1
// ESP8266: Arduino framework
#elif defined(ESP8266) || defined(ARDUINO_ARCH_ESP8266)
    #undef  RAWR_PLATFORM_ESP8266
    #define RAWR_PLATFORM_ESP8266 1
// Apple: requires TargetConditionals.h to distinguish iOS from macOS
#elif defined(__APPLE__)
    #if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
        #undef  RAWR_PLATFORM_MACOS
        #define RAWR_PLATFORM_MACOS 1
    #elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || \
          defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)       || \
          defined(__ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__)
        #undef  RAWR_PLATFORM_IOS
        #define RAWR_PLATFORM_IOS 1
    #else
        #undef  RAWR_PLATFORM_UNKNOWN
        #define RAWR_PLATFORM_UNKNOWN 1
    #endif
// Android defines both __ANDROID__ and __linux__ — must come before Linux
#elif defined(__ANDROID__)
    #undef  RAWR_PLATFORM_ANDROID
    #define RAWR_PLATFORM_ANDROID 1
#elif defined(__linux__)
    #undef  linux // Some compilers define 'linux' as a bare macro alongside '__linux__',
                  // truly god has failed us.
    #undef  RAWR_PLATFORM_LINUX
    #define RAWR_PLATFORM_LINUX 1
// Windows: _WIN32 is defined on both 32 and 64-bit Windows including under MinGW and Cygwin.
// Cygwin and MinGW are distinguished at the ENV layer, not the HOST layer.
// HOST_WINDOWS means "running on Windows hardware", regardless of compatibility layer.
#elif defined(_WIN32) || defined(_WIN64)
    #undef  RAWR_PLATFORM_WINDOWS
    #define RAWR_PLATFORM_WINDOWS 1
// STM32: ST HAL or CubeMX defines USE_HAL_DRIVER; bare CMSIS defines STM32 family macros.
// STM32F0..STM32WL cover the main families.
#elif defined(USE_HAL_DRIVER)           || \
      defined(STM32F0) || defined(STM32F1) || defined(STM32F2) || \
      defined(STM32F3) || defined(STM32F4) || defined(STM32F7) || \
      defined(STM32G0) || defined(STM32G4) || \
      defined(STM32H7) || defined(STM32L0) || defined(STM32L1) || \
      defined(STM32L4) || defined(STM32L5) || \
      defined(STM32U5) || defined(STM32WB) || defined(STM32WL)
    #undef  RAWR_PLATFORM_STM32
    #define RAWR_PLATFORM_STM32 1
// Nordic nRF: family macros cover nRF51 and nRF52 series including all variants.
// nRF9160 is a separate SiP but same SDK — include NRF9160 if needed.
#elif defined(NRF51) || defined(NRF52) || defined(NRF9160)
    #undef  RAWR_PLATFORM_NORDIC
    #define RAWR_PLATFORM_NORDIC 1
// Raspberry Pi Pico: Pico SDK defines PICO_BOARD; some configs define RASPBERRYPI_PICO
#elif defined(PICO_BOARD) || defined(RASPBERRYPI_PICO)
    #undef  RAWR_PLATFORM_PICO
    #define RAWR_PLATFORM_PICO 1
// Teensy: TEENSYDUINO covers all Teensy boards (3.x, 4.x) when using Teensyduino.
// Bare metal Teensy 4.x: __IMXRT1062__. Bare metal Teensy 3.x: __MK*__ variants.
#elif defined(TEENSYDUINO) || defined(__IMXRT1062__) || \
      defined(__MK20DX128__) || defined(__MK20DX256__) || \
      defined(__MK64FX512__) || defined(__MK66FX1M0__)
    #undef  RAWR_PLATFORM_TEENSY
    #define RAWR_PLATFORM_TEENSY 1
#elif defined(__AVR__)
    #undef  RAWR_PLATFORM_AVR
    #define RAWR_PLATFORM_AVR 1
#elif defined(__FreeBSD__)
    #undef  RAWR_PLATFORM_FREEBSD
    #define RAWR_PLATFORM_FREEBSD 1
#elif defined(__OpenBSD__)
    #undef  RAWR_PLATFORM_OPENBSD
    #define RAWR_PLATFORM_OPENBSD 1
#elif defined(__NetBSD__)
    #undef  RAWR_PLATFORM_NETBSD
    #define RAWR_PLATFORM_NETBSD 1
#else
    #undef  RAWR_PLATFORM_UNKNOWN
    #define RAWR_PLATFORM_UNKNOWN 1
#endif
