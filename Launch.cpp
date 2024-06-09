#include "Utility/Launch.h"
#include "spdlog/spdlog.h"
#include <cassert>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <windows.h>

namespace utility {
std::string locateProgram(const std::string &name) {
  std::string path;

  const unsigned buf_size = std::max<unsigned>(32, name.size() + 16);
  std::unique_ptr<char[]> buf;

  try {
    buf = std::make_unique<char[]>(buf_size);
  } catch (const std::exception &e) {
    spdlog::error("make_unique failed: {}", e.what());
    return path;
  }

  // locate cmd

  char cmd_path[32]{};
  size_t env_size = 0;
  auto error = getenv_s(&env_size, cmd_path, "ComSpec");
  if (error) {
    spdlog::error("getenv_s failed: {}", error);
    return path;
  }

  // command line

  char *const buf_ptr = buf.get();
  memset(buf_ptr, 0, buf_size);
  snprintf(buf_ptr, buf_size, "/c \"where %s\"", name.c_str());

  int exit_code =
      launchHiddenProgram(cmd_path, buf_ptr, [&path](std::string &&output) {
        path.append(output);
      });

  if (exit_code) {
    spdlog::error("cmd exits with code {}", exit_code);
    path.clear();
  }

  return path;
}

int waitForExitCode(const std::string &path, char *arg) {
  return launchHiddenProgram(path, arg, [](std::string &&) {});
}

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

  spdlog::debug("process {} launched", pi.dwProcessId);

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

  DWORD exit_code = 0;
  if (!GetExitCodeProcess(pi.hProcess, &exit_code))
    spdlog::error("GetExitCodeProcess failed ({})", GetLastError());
  else
    spdlog::debug("Process exited with code: {}", exit_code);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  CloseHandle(hChildStd_OUT_Rd);

  return exit_code;
}
} // namespace utility
