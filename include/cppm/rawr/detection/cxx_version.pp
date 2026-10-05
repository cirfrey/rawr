#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.detection.cxx_version.pp
#endif

// MSVC doesn't update __cplusplus without /Zc:__cplusplus.
// _MSVC_LANG is always correct on MSVC regardless.
#if defined(_MSVC_LANG)
    #define RAWR_CXX_VERSION _MSVC_LANG
#else
    #define RAWR_CXX_VERSION __cplusplus
#endif
// NOLINTBEGIN(modernize-macro-to-enum)
#define RAWR_CXX_VERSION_98 199711L
#define RAWR_CXX_VERSION_11 201103L
#define RAWR_CXX_VERSION_14 201402L
#define RAWR_CXX_VERSION_17 201703L
#define RAWR_CXX_VERSION_20 202002L
#define RAWR_CXX_VERSION_23 202302L
#define RAWR_CXX_VERSION_26 202603L
// NOLINTEND(modernize-macro-to-enum)
