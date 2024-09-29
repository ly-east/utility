#include "Utility/Crypto/Hash.h"
#include "Utility/FileSystem/Filesystem.h"
#include "Utility/Json/AriaRpc.h"
#include "Utility/Json/Json.h"
#include "Utility/Log/Logger.h"
#include "Utility/Math/Random.h"
#include "Utility/Network/Socket.h"
#include "Utility/Network/UserAgent.h"
#include "Utility/Process/Launch.h"
#include "Utility/String/Encoding.h"
#include "Utility/String/Format.h"
#include "Utility/String/Replace.h"
#include "Utility/String/Trim.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

#if defined(_WIN32)
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }

  return TRUE; // Successful DLL_PROCESS_ATTACH.
}
#elif defined(__linux__)
void __attribute__((constructor)) my_library_init(void) {}

void __attribute__((destructor)) my_library_fini(void) {}
#endif
