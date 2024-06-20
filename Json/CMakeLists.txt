add_library(BBDownUtilityJson STATIC
  AriaRpc.cpp
  Json.cpp
)

target_include_directories(BBDownUtilityJson PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(BBDownUtilityJson
  nlohmann_json
  spdlog
)
