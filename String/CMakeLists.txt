add_library(BBDownUtilityString STATIC
  Trim.cpp
)

target_include_directories(BBDownUtilityString PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(BBDownUtilityString
  spdlog
)
