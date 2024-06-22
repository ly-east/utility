add_library(BBDownUtilityNetwork STATIC
  Socket.cpp
  UserAgent.cpp
)

target_include_directories(BBDownUtilityNetwork PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(BBDownUtilityNetwork
  BBDownUtilityMath
  spdlog
)
