# ===================================================================
# Utility test suite
# ===================================================================

add_executable(tester
  UtilityCrypto.cpp
  UtilityEventBus.cpp
  UtilityFilesystem.cpp
  UtilityJson.cpp
  UtilityMath.cpp
  UtilityProcess.cpp
  UtilityString.cpp
)

add_test(NAME tester COMMAND tester)

target_include_directories(tester
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_SOURCE_DIR}/include
  PUBLIC ${CMAKE_SOURCE_DIR}/third-party/gtest/include
)

target_link_libraries(tester
  Utility
  gtest
  gtest_main
  ulog::ulog
)
