add_subdirectory(Crypto)
add_subdirectory(Json)
add_subdirectory(Process)
add_subdirectory(String)

add_library(BBDownUtility STATIC
  Filesystem.cpp
  Logger.cpp
  Network.cpp
  Random.cpp
)

target_include_directories(BBDownUtility PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(BBDownUtility
  spdlog
)
