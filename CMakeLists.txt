cmake_minimum_required(VERSION 3.29)

set(
  CMAKE_MSVC_RUNTIME_LIBRARY
  "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
set(CMAKE_C_FLAGS_MINSIZEREL "/O1 /Ob2 /DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "/O1 /Ob2 /DNDEBUG")

project(chrome_plus LANGUAGES C CXX RC)

if(NOT WIN32)
  message(FATAL_ERROR "Chrome++ Next only supports Windows.")
endif()

if(NOT MSVC)
  message(
    FATAL_ERROR
    "Chrome++ Next requires an MSVC-compatible compiler frontend."
  )
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_VS_PLATFORM_NAME)
  string(TOLOWER "${CMAKE_VS_PLATFORM_NAME}" CHROME_PLUS_ARCH)
  if(CHROME_PLUS_ARCH STREQUAL "win32")
    set(CHROME_PLUS_ARCH x86)
  endif()
else()
  set(CHROME_PLUS_ARCH "" CACHE STRING "Target architecture")
  set_property(CACHE CHROME_PLUS_ARCH PROPERTY STRINGS x86 x64 arm64)
endif()

if(NOT CHROME_PLUS_ARCH MATCHES "^(x86|x64|arm64)$")
  message(FATAL_ERROR "CHROME_PLUS_ARCH must be x86, x64, or arm64.")
endif()

option(
  CHROME_PLUS_USE_VC_LTL
  "Use VC-LTL for optimized configurations"
  OFF
)

add_compile_definitions(
  UNICODE
  _UNICODE
  $<$<CONFIG:Debug>:_DEBUG>
)

add_compile_options(
  $<$<COMPILE_LANGUAGE:C,CXX>:/W3>
  $<$<COMPILE_LANGUAGE:C,CXX>:/source-charset:utf-8>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:/EHs-c->
)

add_library(detours STATIC
  detours/src/detours.cpp
  detours/src/disasm.cpp
  detours/src/image.cpp
  detours/src/modules.cpp
)

target_include_directories(detours PUBLIC detours/src)
target_compile_options(detours PRIVATE
  $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wno-unknown-pragmas>
  $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wno-reorder-ctor>
  $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wno-unused-local-typedef>
)

if(CHROME_PLUS_ARCH STREQUAL "x86")
  target_compile_definitions(detours PRIVATE _X86_)
  target_sources(detours PRIVATE detours/src/disolx86.cpp)
elseif(CHROME_PLUS_ARCH STREQUAL "x64")
  target_compile_definitions(detours PRIVATE _AMD64_)
  target_sources(detours PRIVATE detours/src/disolx64.cpp)
else()
  target_compile_definitions(detours PRIVATE _ARM64_)
  target_sources(detours PRIVATE detours/src/disolarm64.cpp)
endif()

add_library(mini_gzip STATIC
  mini_gzip/miniz.c
  mini_gzip/mini_gzip.c
)

target_include_directories(mini_gzip PUBLIC mini_gzip)
target_compile_options(mini_gzip PRIVATE
  $<$<COMPILE_LANG_AND_ID:C,MSVC>:/wd5287>
  $<$<COMPILE_LANG_AND_ID:C,MSVC>:/wd4267>
)

add_library(chrome_plus SHARED
  src/appid.cc
  src/chrome++.cc
  src/chrome++.rc
  src/config.cc
  src/green.cc
  src/hijack.cc
  src/hotkey.cc
  src/inputhook.cc
  src/keymapping.cc
  src/pakfile.cc
  src/pakpatch.cc
  src/policies.cc
  src/portable.cc
  src/tabbookmark.cc
  src/uia.cc
  src/upgradenotification.cc
  src/utils.cc
)

target_link_libraries(chrome_plus PRIVATE
  onecore
  propsys
  oleacc
  detours
  mini_gzip
)

target_link_options(chrome_plus PRIVATE
  LINKER:/DYNAMICBASE
  LINKER:/MANIFEST:NO
)

set_target_properties(chrome_plus PROPERTIES OUTPUT_NAME version)

set_property(
  TARGET detours mini_gzip chrome_plus
  PROPERTY INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE
)

if(CHROME_PLUS_USE_VC_LTL)
  include(FetchContent)
  FetchContent_Declare(
    vc_ltl
    URL
      "https://github.com/Chuyu-Team/VC-LTL5/releases/download/v5.3.1/VC-LTL-Binary.7z"
    URL_HASH
      "SHA256=7a18799ed3aa84a225610a5447a56bc534c5c98ccb8dec05caba0e3f633431ad"
  )
  FetchContent_MakeAvailable(vc_ltl)

  set(VC_LTL_EnableCMakeInterface ON)
  if(CHROME_PLUS_ARCH STREQUAL "x86")
    set(LTLPlatform Win32)
  else()
    set(LTLPlatform "${CHROME_PLUS_ARCH}")
  endif()
  if(CHROME_PLUS_ARCH STREQUAL "arm64")
    set(WindowsTargetPlatformMinVersion 10.0.10240.0)
  else()
    set(WindowsTargetPlatformMinVersion 6.0.6000.0)
  endif()

  include("${vc_ltl_SOURCE_DIR}/VC-LTL helper for cmake.cmake")
  target_link_libraries(
    chrome_plus
    PRIVATE "$<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:VC_LTL>"
  )
endif()
