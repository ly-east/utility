add_library(UtilityRaii STATIC
  raii.cpp
)

target_include_directories(UtilityRaii PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
