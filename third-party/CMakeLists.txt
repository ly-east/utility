# ===================================================================
# Google Test
# ===================================================================

set(BUILD_GMOCK OFF)
set(GOOGLETEST_VERSION "1.14.0")
set(GTEST_LIB gtest gtest_main)

add_subdirectory(gtest)

target_compile_definitions(gtest
  PUBLIC GTEST_LINKED_AS_SHARED_LIBRARY=0
  PUBLIC GTEST_CREATE_SHARED_LIBRARY=0
)
