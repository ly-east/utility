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

  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  // create the annonymous pipe
  if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) {
    spdlog::error("CreatePipe failed ({})", GetLastError());
    return 1;
  }

  // let reading handle cannot be inherited
  SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);

  STARTUPINFOA si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = si.dwFlags | STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.hStdError = hChildStd_OUT_Wr; // stderr is redirected also
  si.hStdOutput = hChildStd_OUT_Wr;
  si.wShowWindow = SW_HIDE;

  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

  // const char *cmd = "c:/windows/system32/cmd.exe";
  // char cmdLine[64];
  // sprintf_s(cmdLine, sizeof(cmdLine), "/c \"where %s\"", "python");

  // create subprocess
  if (!CreateProcessA(path.c_str(), arg, NULL, NULL, TRUE, 0, NULL, NULL, &si,
                      &pi)) {
    spdlog::error("CreateProcess failed ({})", GetLastError());
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_OUT_Wr);
    return 1;
  }

  CloseHandle(hChildStd_OUT_Wr);

  char buffer[64];
  DWORD bytesRead = 0;
  std::string output;

  // read from subprocess until pipe is closed
  while (ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer), &bytesRead, NULL) &&
         bytesRead > 0) {
    output.append(buffer, bytesRead);
    spdlog::debug(output);
    output.clear();
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
