add_library(BBDownUtility STATIC
  Filesystem.cpp
  Launch.cpp
  Logger.cpp
  Network.cpp
  Json.cpp
  Random.cpp
  String.cpp
)

target_include_directories(BBDownUtility PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(BBDownUtility
  nlohmann_json
  spdlog
)
