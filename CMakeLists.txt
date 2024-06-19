add_subdirectory(Crypto)
add_subdirectory(String)

add_library(BBDownUtility STATIC
  AriaRpc.cpp
  Filesystem.cpp
  Launch.cpp
  Logger.cpp
  Network.cpp
  Json.cpp
  Random.cpp
)

target_include_directories(BBDownUtility PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(BBDownUtility
  BBDownUtilityString
  nlohmann_json
  spdlog
)
