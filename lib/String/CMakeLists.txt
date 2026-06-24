add_library(UtilityString STATIC
  Encoding.cpp
  Format.cpp
  Match.cpp
  Replace.cpp
  Trim.cpp
)

target_include_directories(UtilityString PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(UtilityString
  ulog::ulog
)
