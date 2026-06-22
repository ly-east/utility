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
