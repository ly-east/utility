add_library(BBDownUtilityLog STATIC
  Logger.cpp
)

target_include_directories(BBDownUtilityLog PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(BBDownUtilityLog
  spdlog
)
