add_library(BBDownUtility STATIC
  Filesystem.cpp
  Logger.cpp
  Json.cpp
  Random.cpp
)

target_include_directories(BBDownUtility PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(BBDownUtility
  nlohmann_json
  spdlog
)
