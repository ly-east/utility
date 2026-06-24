cmake_minimum_required(VERSION 3.15.0)

# ref: https://cmake.org/cmake/help/latest/prop_tgt/MSVC_RUNTIME_LIBRARY.html
if (POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif (POLICY CMP0091)

project(Utility VERSION 1.9.8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
  if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
  endif(MSVC)
endif(WIN32)


# ===================================================================
# Dependencies and subdirectories

# Dependencies may already be available as targets via parent-project
# add_subdirectory; otherwise try find_package. If neither works,
# add submodules or use FetchContent.
if(NOT TARGET OpenSSL::Crypto)
  find_package(OpenSSL REQUIRED)
endif()

if(NOT TARGET nlohmann_json)
  find_package(nlohmann_json QUIET)
  if(NOT nlohmann_json_FOUND)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third-party/json/CMakeLists.txt)
      message(STATUS "nlohmann_json not installed — using submodule")
      add_subdirectory(third-party/json)
    else()
      message(FATAL_ERROR
        "nlohmann_json not found.\n"
        "Options:\n"
        "  1. vcpkg:    vcpkg install nlohmann-json\n"
        "  2. Submodule: git submodule add https://github.com/nlohmann/json.git third-party/json\n"
        "     cd third-party/json && git checkout v3.11.3"
      )
    endif()
  endif()
endif()

if(NOT TARGET ulog::ulog)
  find_package(ulog REQUIRED)
endif()


# ===================================================================
# Utility dynamic link library

# All static libs are components of the Utility DLL — symbols must be dllexport
add_compile_definitions(UTILITY_DLL=1)

add_subdirectory(lib)
add_library(Utility SHARED lib/Utility.cpp)

target_compile_definitions(Utility PRIVATE UTILITY_DLL=1)
target_include_directories(Utility PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(Utility
  UtilityCrypto
  UtilityEventBus
  UtilityFileSystem
  UtilityJson
  UtilityMath
  UtilityNetwork
  UtilityProcess
  UtilityRaii
  UtilityString
  OpenSSL::Crypto
  ulog::ulog
)

# MSVC linker only pulls objects from static libs that resolve an undefined
# symbol. Since Utility.cpp only has DllMain and references none of the
# __declspec(dllexport) functions, the linker would skip every static lib,
# produce an empty DLL, and omit the import library. /WHOLEARCHIVE forces
# all objects in, so the dllexport markers take effect.
if(MSVC)
  foreach(lib IN ITEMS
      UtilityCrypto UtilityEventBus UtilityFileSystem UtilityJson
      UtilityMath UtilityNetwork UtilityProcess UtilityRaii UtilityString)
    target_link_options(Utility PRIVATE "/WHOLEARCHIVE:$<TARGET_LINKER_FILE:${lib}>")
  endforeach()
endif()

install(TARGETS Utility EXPORT utilityTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
