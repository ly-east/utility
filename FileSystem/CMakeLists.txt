add_library(BBDownUtilityFileSystem STATIC
  Filesystem.cpp
)

target_include_directories(BBDownUtilityFileSystem PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(BBDownUtilityFileSystem
  spdlog
)
