# ===================================================================
# Utility test suite
# ===================================================================

# gtest is provided either by the parent project or by
# third-party/CMakeLists.txt in standalone builds.
if(NOT TARGET gtest)
  message(FATAL_ERROR "gtest target not available")
endif()

# Named utility_tester to avoid clashing with the parent project's own
# `tester` target when Utility is built as a subproject.
add_executable(utility_tester
  UtilityCrypto.cpp
  UtilityEventBus.cpp
  UtilityFilesystem.cpp
  UtilityJson.cpp
  UtilityMath.cpp
  UtilityProcess.cpp
  UtilityString.cpp
)

add_test(NAME utility_tester COMMAND utility_tester)

target_include_directories(utility_tester
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(utility_tester
  Utility
  gtest
  gtest_main
  ulog::ulog
)
