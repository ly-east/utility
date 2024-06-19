add_library(BBDownUtilityCrypto STATIC
  Hash.cpp
)

target_include_directories(BBDownUtilityCrypto PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${OPENSSL_ROOT_DIR}/include
)

target_link_libraries(BBDownUtilityCrypto
  BBDownUtilityString
  ${OpenSSL_CRYPTO_LIBRARIES}
  spdlog
)
