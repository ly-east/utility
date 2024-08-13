#include "Utility/Process/Launch.h"
#include "Utility/String/Encoding.h"
#include "Utility/String/Trim.h"
#include "spdlog/spdlog.h"
#include <Windows.h>
#include <cassert>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>

namespace utility {
namespace process {
// Type of CreateProcess caller function
using CrtFuncTy = std::function<bool(HANDLE, PROCESS_INFORMATION &)>;

int launchHiddenProgram(CrtFuncTy caller, RdtCbFuncTy func,
                        std::promise<bool> *p);

std::string getExecutableFilePath(const std::string &name) {
  std::filesystem::path p{std::filesystem::current_path()};
  p.append(name);
  if (!p.has_extension())
    p.replace_extension(".exe");

  // Program at the same path of BBDown is preferred to use
  if (std::filesystem::exists(p) && std::filesystem::is_regular_file(p))
    return p.string();

  return locateProgram(name);
}

std::string locateProgram(const std::string &name) {
  using namespace utility::string;

  std::u16string wide_name{utf8ToUtf16(ansiToUtf8(name))};
  std::string path;

  // locate cmd

  wchar_t cmd_path[64]{};
  size_t env_size = 0;
  auto error = _wgetenv_s(&env_size, cmd_path, L"ComSpec");
  if (error) {
    spdlog::error("getenv_s failed: {}", error);
    return path;
  }

  // command line

  const unsigned buf_size =
      std::max<unsigned>(64, (unsigned)wide_name.size() + 16);
  std::unique_ptr<wchar_t[]> buf;

  try {
    buf = std::make_unique<wchar_t[]>(buf_size);
  } catch (const std::exception &e) {
    spdlog::error("make_unique failed: {}", e.what());
    return path;
  }

  wchar_t *const buf_ptr = buf.get();
  _snwprintf(buf_ptr, buf_size, L"/c \"where %s\"",
             (wchar_t *)wide_name.c_str());

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

int launchHiddenProgram(const std::string &path, char *arg, RdtCbFuncTy func,
                        std::promise<bool> *p) {
  auto caller = [path, arg](HANDLE writer, PROCESS_INFORMATION &pi) -> bool {
    STARTUPINFOA si{sizeof(STARTUPINFO)};

    si.dwFlags = si.dwFlags | STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdError = writer; // stderr is redirected also
    si.hStdOutput = writer;
    si.wShowWindow = SW_HIDE;

    return CreateProcessA(path.c_str(), arg, NULL, NULL, TRUE, 0, NULL, NULL,
                          &si, &pi);
  };

  return launchHiddenProgram(caller, func, p);
}

int launchHiddenProgram(const std::wstring &path, wchar_t *arg,
                        RdtCbFuncTy func, std::promise<bool> *p) {
  auto caller = [path, arg](HANDLE writer, PROCESS_INFORMATION &pi) -> bool {
    STARTUPINFOW si{sizeof(STARTUPINFO)};

    si.dwFlags = si.dwFlags | STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdError = writer; // stderr is redirected also
    si.hStdOutput = writer;
    si.wShowWindow = SW_HIDE;

    return CreateProcessW(path.c_str(), arg, NULL, NULL, TRUE, 0, NULL, NULL,
                          &si, &pi);
  };

  return launchHiddenProgram(caller, func, p);
}

int launchHiddenProgram(CrtFuncTy caller, RdtCbFuncTy func,
                        std::promise<bool> *p) {
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

  PROCESS_INFORMATION pi{};

  // create subprocess
  if (!caller(hChildStd_OUT_Wr, pi)) {
    spdlog::error("CreateProcess failed ({})", GetLastError());
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_OUT_Wr);

    if (p)
      p->set_value(false);
    return 1;
  }

  spdlog::debug("process {} launched", pi.dwProcessId);
  if (p)
    p->set_value(true);

  CloseHandle(hChildStd_OUT_Wr);

  DWORD buf_size = 128;
  std::unique_ptr<char[]> buf = std::make_unique<char[]>(buf_size);
  char *const buf_ptr = buf.get();
  DWORD byte_read = 0;

  // read from subprocess until pipe is closed
  while (ReadFile(hChildStd_OUT_Rd, buf_ptr, buf_size, &byte_read, NULL) &&
         byte_read > 0) {
    // Content read in buffer is encoded by ANSI
    auto output{utility::string::trim(std::string(buf_ptr, byte_read))};

    if (output.empty())
      continue;

    func(std::move(output));
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
} // namespace process
} // namespace utility
