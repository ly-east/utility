add_library(Utility SHARED
  Crash/CrashPad.cpp
  Crypto/Hash.cpp
  FileSystem/DumpFile.cpp
  FileSystem/Filesystem.cpp
  Json/AriaRpc.cpp
  Json/Json.cpp
  Math/Random.cpp
  Network/Socket.cpp
  Network/UserAgent.cpp
  Process/Launch.cpp
  Raii/Raii.cpp
  String/Encoding.cpp
  String/Format.cpp
  String/Replace.cpp
  String/Trim.cpp
  Utility.cpp
)

target_compile_definitions(Utility PUBLIC BBDOWN_DLL=1)

target_include_directories(Utility PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${OPENSSL_ROOT_DIR}/include
  ${CMAKE_SOURCE_DIR}/third-party/crashpad
  ${CMAKE_SOURCE_DIR}/third-party/ulog/include
)

target_link_libraries(Utility
  ${OPENSSL_CRYPTO_LIBRARIES}
  client
  nlohmann_json
  ulog
)

install(TARGETS Utility)
