add_library(UtilityProcess STATIC
  Launch.cpp
  Terminate.cpp
)

target_include_directories(UtilityProcess PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(UtilityProcess
  ulog::ulog
)
