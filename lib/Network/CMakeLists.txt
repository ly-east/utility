add_library(UtilityNetwork STATIC
  Socket.cpp
  UserAgent.cpp
)

target_include_directories(UtilityNetwork PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(UtilityNetwork
  ulog::ulog
)
