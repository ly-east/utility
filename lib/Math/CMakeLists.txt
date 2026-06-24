add_library(UtilityMath STATIC
  Random.cpp
)

target_include_directories(UtilityMath PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
