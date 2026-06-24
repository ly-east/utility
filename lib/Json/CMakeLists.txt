add_library(UtilityJson STATIC
  AriaRpc.cpp
  Json.cpp
)

target_include_directories(UtilityJson PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(UtilityJson
  nlohmann_json
  ulog::ulog
)
