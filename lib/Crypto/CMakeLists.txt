add_library(UtilityCrypto STATIC
  Hash.cpp
)

target_include_directories(UtilityCrypto PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(UtilityCrypto
  OpenSSL::Crypto
  ulog::ulog
)
