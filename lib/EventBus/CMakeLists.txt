add_library(UtilityEventBus STATIC
  EventBus.cpp
)

target_include_directories(UtilityEventBus PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(UtilityEventBus
  ulog::ulog
)
