#include "Utility/Launch.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <string>
#include <windows.h>

namespace utility {
std::string locateProgram(const std::string &name) { return std::string(); }

int waitForExitCode(const std::string &path, char *arg) { return 0; }

int launchHiddenProgram(const std::string &path, char *arg, RdtCbFuncTy func) {
  HANDLE hChildStd_OUT_Rd = NULL;
  HANDLE hChildStd_OUT_Wr = NULL;

  SECURITY_ATTRIBUTES attr{sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

  // create the annonymous pipe
  if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &attr, 0)) {
    spdlog::error("CreatePipe failed ({})", GetLastError());
    return 1;
  }

  // let reading handle cannot be inherited
  SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);

  STARTUPINFOA si{sizeof(STARTUPINFO)};
  PROCESS_INFORMATION pi{};

  si.dwFlags = si.dwFlags | STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.hStdError = hChildStd_OUT_Wr; // stderr is redirected also
  si.hStdOutput = hChildStd_OUT_Wr;
  si.wShowWindow = SW_HIDE;

  // create subprocess
  if (!CreateProcessA(path.c_str(), arg, NULL, NULL, TRUE, 0, NULL, NULL, &si,
                      &pi)) {
    spdlog::error("CreateProcess failed ({})", GetLastError());
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_OUT_Wr);
    return 1;
  }

  CloseHandle(hChildStd_OUT_Wr);

  char buffer[32];
  DWORD byte_read = 0;

  // read from subprocess until pipe is closed
  while (ReadFile(hChildStd_OUT_Rd, buffer, sizeof buffer, &byte_read, NULL) &&
         byte_read > 0) {
    if (buffer[byte_read - 1] == '\n' && byte_read >= 2)
      byte_read -= 2; // remove tailing \r\n

    func(std::string(buffer, byte_read));
  }

  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD exitCode = 0;
  if (!GetExitCodeProcess(pi.hProcess, &exitCode))
    spdlog::error("GetExitCodeProcess failed ({})", GetLastError());
  else
    spdlog::debug("Process exited with code: {}", exitCode);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  CloseHandle(hChildStd_OUT_Rd);

  return 0;
}
} // namespace utility
