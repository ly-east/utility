add_library(UtilityFileSystem STATIC
  DumpFile.cpp
  Filesystem.cpp
)

target_include_directories(UtilityFileSystem PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(UtilityFileSystem
  ulog::ulog
)
