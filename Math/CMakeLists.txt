add_library(BBDownUtilityMath STATIC
  Random.cpp
)

target_include_directories(BBDownUtilityMath PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)
