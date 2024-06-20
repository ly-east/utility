add_library(BBDownUtilityProcess STATIC
  Launch.cpp
)

target_include_directories(BBDownUtilityProcess PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(BBDownUtilityProcess
  BBDownUtilityString
  spdlog
)
