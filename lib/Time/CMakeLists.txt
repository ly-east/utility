add_library(UtilityTime STATIC
  UnixTimestamp.cpp
)

target_include_directories(UtilityTime PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

