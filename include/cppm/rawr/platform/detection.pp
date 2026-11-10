#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.platform.detection.pp
#endif

#include "rawr/distribution/todo.pp"

RAWR_TODO("detect wasi")

// --- Ids. ---

#define RAWR_PLATFORMID_UNKNOWN        1
#define RAWR_PLATFORMID_LINUX          2
#define RAWR_PLATFORMID_WINDOWS        3
#define RAWR_PLATFORMID_FREEBSD        4
#define RAWR_PLATFORMID_OPENBSD        5
#define RAWR_PLATFORMID_NETBSD         6
#define RAWR_PLATFORMID_MACOS          7
#define RAWR_PLATFORMID_IOS            8
#define RAWR_PLATFORMID_ANDROID        9
#define RAWR_PLATFORMID_WASM           10
#define RAWR_PLATFORMID_ESP32          11
#define RAWR_PLATFORMID_ESP8266        12
#define RAWR_PLATFORMID_STM32          13
#define RAWR_PLATFORMID_NORDIC         14
#define RAWR_PLATFORMID_PICO           15
#define RAWR_PLATFORMID_TEENSY         16
#define RAWR_PLATFORMID_AVR            17
#define RAWR_PLATFORMID_MAX            17

// --- Detection. ---

// WASM: both Emscripten and standalone WASM runtimes
#if defined(__EMSCRIPTEN__) || defined(__wasm__)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_WASM
// ESP32: ESP-IDF defines ESP_PLATFORM; Arduino framework defines ARDUINO_ARCH_ESP32
#elif defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP32)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_ESP32
// ESP8266: Arduino framework
#elif defined(ESP8266) || defined(ARDUINO_ARCH_ESP8266)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_ESP8266
// Apple: requires TargetConditionals.h to distinguish iOS from macOS
#elif defined(__APPLE__)
    #if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
        #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_MACOS
    #elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || \
          defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)       || \
          defined(__ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__)
        #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_IOS
    #else
        #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_UNKNOWN
    #endif
// Android defines both __ANDROID__ and __linux__ — must come before Linux
#elif defined(__ANDROID__)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_ANDROID
#elif defined(__linux__)
    #undef  linux // Some compilers define 'linux' as a bare macro alongside '__linux__',
                  // truly god has failed us.
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_LINUX
// Windows: _WIN32 is defined on both 32 and 64-bit Windows including under MinGW and Cygwin.
// Cygwin and MinGW are distinguished at the ENV layer, not the HOST layer.
// HOST_WINDOWS means "running on Windows hardware", regardless of compatibility layer.
#elif defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)  // Cygwin defines no _WIN32; the ENV axis says Cygwin
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_WINDOWS
// STM32: ST HAL or CubeMX defines USE_HAL_DRIVER; bare CMSIS defines STM32 family macros.
// STM32F0..STM32WL cover the main families.
#elif defined(USE_HAL_DRIVER)           || \
      defined(STM32F0) || defined(STM32F1) || defined(STM32F2) || \
      defined(STM32F3) || defined(STM32F4) || defined(STM32F7) || \
      defined(STM32G0) || defined(STM32G4) || \
      defined(STM32H7) || defined(STM32L0) || defined(STM32L1) || \
      defined(STM32L4) || defined(STM32L5) || \
      defined(STM32U5) || defined(STM32WB) || defined(STM32WL)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_STM32
// Nordic nRF: family macros cover nRF51 and nRF52 series including all variants.
// nRF9160 is a separate SiP but same SDK — include NRF9160 if needed.
#elif defined(NRF51) || defined(NRF52) || defined(NRF9160)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_NORDIC
// Raspberry Pi Pico: Pico SDK defines PICO_BOARD; some configs define RASPBERRYPI_PICO
#elif defined(PICO_BOARD) || defined(RASPBERRYPI_PICO)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_PICO
// Teensy: TEENSYDUINO covers all Teensy boards (3.x, 4.x) when using Teensyduino.
// Bare metal Teensy 4.x: __IMXRT1062__. Bare metal Teensy 3.x: __MK*__ variants.
#elif defined(TEENSYDUINO) || defined(__IMXRT1062__) || \
      defined(__MK20DX128__) || defined(__MK20DX256__) || \
      defined(__MK64FX512__) || defined(__MK66FX1M0__)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_TEENSY
#elif defined(__AVR__)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_AVR
#elif defined(__FreeBSD__)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_FREEBSD
#elif defined(__OpenBSD__)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_OPENBSD
#elif defined(__NetBSD__)
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_NETBSD
#else
    #define RAWR_DETECTED_PLATFORMID RAWR_PLATFORMID_UNKNOWN
#endif

// --- User override. ---

// -DRAWR_SET_PLATFORMID=RAWR_PLATFORMID_<VALUE> replaces detection (UNKNOWN is a legal value). RAWR_DETECTED_PLATFORMID always keeps what
// detection found. Id 0 is reserved as "malformed": a misspelled name evaluates to 0.
#ifdef RAWR_PLATFORMID
    #error "rawr: RAWR_PLATFORMID is an output of detection; to override it define RAWR_SET_PLATFORMID"
#endif
#ifdef RAWR_SET_PLATFORMID
    #if RAWR_SET_PLATFORMID < 1 || RAWR_SET_PLATFORMID > RAWR_PLATFORMID_MAX
        #error "rawr: RAWR_SET_PLATFORMID must be one of the RAWR_PLATFORMID_* values (a misspelled name evaluates to 0, which is never valid)"
    #endif
    #define RAWR_PLATFORMID           RAWR_SET_PLATFORMID
    #define RAWR_PLATFORMID_DETECTED  0
    #define RAWR_PLATFORMID_OVERRIDEN 1
#else
    #define RAWR_PLATFORMID           RAWR_DETECTED_PLATFORMID
    #define RAWR_PLATFORMID_DETECTED  1
    #define RAWR_PLATFORMID_OVERRIDEN 0
#endif

// --- Canonical macros. ---

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
#if RAWR_PLATFORMID == RAWR_PLATFORMID_UNKNOWN
    #undef  RAWR_PLATFORM_UNKNOWN
    #define RAWR_PLATFORM_UNKNOWN 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_LINUX
    #undef  RAWR_PLATFORM_LINUX
    #define RAWR_PLATFORM_LINUX 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_WINDOWS
    #undef  RAWR_PLATFORM_WINDOWS
    #define RAWR_PLATFORM_WINDOWS 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_FREEBSD
    #undef  RAWR_PLATFORM_FREEBSD
    #define RAWR_PLATFORM_FREEBSD 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_OPENBSD
    #undef  RAWR_PLATFORM_OPENBSD
    #define RAWR_PLATFORM_OPENBSD 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_NETBSD
    #undef  RAWR_PLATFORM_NETBSD
    #define RAWR_PLATFORM_NETBSD 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_MACOS
    #undef  RAWR_PLATFORM_MACOS
    #define RAWR_PLATFORM_MACOS 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_IOS
    #undef  RAWR_PLATFORM_IOS
    #define RAWR_PLATFORM_IOS 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_ANDROID
    #undef  RAWR_PLATFORM_ANDROID
    #define RAWR_PLATFORM_ANDROID 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_WASM
    #undef  RAWR_PLATFORM_WASM
    #define RAWR_PLATFORM_WASM 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_ESP32
    #undef  RAWR_PLATFORM_ESP32
    #define RAWR_PLATFORM_ESP32 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_ESP8266
    #undef  RAWR_PLATFORM_ESP8266
    #define RAWR_PLATFORM_ESP8266 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_STM32
    #undef  RAWR_PLATFORM_STM32
    #define RAWR_PLATFORM_STM32 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_NORDIC
    #undef  RAWR_PLATFORM_NORDIC
    #define RAWR_PLATFORM_NORDIC 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_PICO
    #undef  RAWR_PLATFORM_PICO
    #define RAWR_PLATFORM_PICO 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_TEENSY
    #undef  RAWR_PLATFORM_TEENSY
    #define RAWR_PLATFORM_TEENSY 1
#elif RAWR_PLATFORMID == RAWR_PLATFORMID_AVR
    #undef  RAWR_PLATFORM_AVR
    #define RAWR_PLATFORM_AVR 1
#endif
