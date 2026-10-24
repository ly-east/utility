cmake_minimum_required(VERSION 3.15.0)

# ref: https://cmake.org/cmake/help/latest/prop_tgt/MSVC_RUNTIME_LIBRARY.html
if (POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif (POLICY CMP0091)

project(Utility VERSION 1.9.8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Tests are built by default for standalone builds; when Utility is embedded
# as a subproject the parent decides through UTILITY_BUILD_TESTS.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(UTILITY_BUILD_TESTS_DEFAULT ON)
else()
  set(UTILITY_BUILD_TESTS_DEFAULT OFF)
endif()
option(UTILITY_BUILD_TESTS "Build tests" ${UTILITY_BUILD_TESTS_DEFAULT})

if(WIN32)
  if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
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
# Third-party dependencies

add_subdirectory(third-party)

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
# GNU ld has the same behavior; --whole-archive is its equivalent.
if(MSVC)
  foreach(lib IN ITEMS
      UtilityCrypto UtilityEventBus UtilityFileSystem UtilityJson
      UtilityMath UtilityNetwork UtilityProcess UtilityRaii UtilityString)
    target_link_options(Utility PRIVATE "/WHOLEARCHIVE:$<TARGET_LINKER_FILE:${lib}>")
  endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_link_options(Utility PRIVATE
    "-Wl,--whole-archive"
    $<TARGET_FILE:UtilityCrypto>
    $<TARGET_FILE:UtilityEventBus>
    $<TARGET_FILE:UtilityFileSystem>
    $<TARGET_FILE:UtilityJson>
    $<TARGET_FILE:UtilityMath>
    $<TARGET_FILE:UtilityNetwork>
    $<TARGET_FILE:UtilityProcess>
    $<TARGET_FILE:UtilityRaii>
    $<TARGET_FILE:UtilityString>
    "-Wl,--no-whole-archive"
  )
endif()

install(TARGETS Utility EXPORT utilityTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ===================================================================
# Tests

if(UTILITY_BUILD_TESTS)
  add_subdirectory(test)
endif()
