#include "Utility/Process/Launch.h"
#include "Utility/String/Encoding.h"
#include "Utility/String/Trim.h"
#include "ulog/ulog.h"
#include <cassert>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <memory>

#if defined(_WIN32)
#include <Windows.h>

// Type of CreateProcess caller function
using CrtFuncTy = std::function<bool(HANDLE, PROCESS_INFORMATION &)>;
#elif defined(__linux__)
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace {
#if defined(_WIN32)
int launchHiddenProgramWindows(CrtFuncTy caller,
                               utility::process::RdtCbFunc func,
                               std::promise<bool> *p) {
  HANDLE hChildStd_OUT_Rd = NULL;
  HANDLE hChildStd_OUT_Wr = NULL;

  SECURITY_ATTRIBUTES attr{sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

  // create the annonymous pipe
  if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &attr, 0)) {
    ulg.error("CreatePipe failed ({})", GetLastError());
    return 1;
  }

  // let reading handle cannot be inherited
  SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);

  PROCESS_INFORMATION pi{};

  // create subprocess
  if (!caller(hChildStd_OUT_Wr, pi)) {
    ulg.error("CreateProcess failed ({})", GetLastError());
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_OUT_Wr);

    if (p)
      p->set_value(false);
    return 1;
  }

  ulg.debug("process {} launched", pi.dwProcessId);
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
    ulg.error("GetExitCodeProcess failed ({})", GetLastError());
  else
    ulg.debug("Process exited with code: {}", exit_code);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  CloseHandle(hChildStd_OUT_Rd);

  return exit_code;
}

#elif defined(__linux__)

#endif
} // namespace

namespace utility {
namespace process {
utility::string::Str getExecutableFilePath(const utility::string::Str &name) {
  std::filesystem::path p{std::filesystem::current_path()};

  try {
    p.append(name);

#if defined(_WIN32)
    if (!p.has_extension())
      p.replace_extension(".exe");
#endif
  } catch (const std::exception &e) {
    ulg.error("getExecutableFilePath: {}", e.what());
    return {};
  }

  // Program at the same path of BBDown is preferred to use
  if (std::filesystem::exists(p) && std::filesystem::is_regular_file(p))
#if defined(_WIN32)
    return p.wstring();
#elif defined(__linux__)
    return p.string();
#endif

  return locateProgram(name);
}

#if defined(_WIN32)

int launchHiddenProgram(const utility::string::Str &path,
                        utility::string::Char *arg, RdtCbFunc func,
                        std::promise<bool> *p) {
  auto caller = [path, arg](HANDLE writer, PROCESS_INFORMATION &pi) -> bool {
    STARTUPINFOW si{sizeof(STARTUPINFO)};

    si.dwFlags = si.dwFlags | STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdError = writer; // stderr is redirected also
    si.hStdOutput = writer;
    si.wShowWindow = SW_HIDE;

    return CreateProcessW(path.c_str(), arg, NULL, NULL, TRUE, 0, NULL, NULL,
                          &si, &pi);
  };

  return ::launchHiddenProgramWindows(caller, func, p);
}

utility::string::Str locateProgram(const utility::string::Str &name) {
  using namespace utility::string;

  std::string path;

  // locate cmd

  wchar_t cmd_path[64]{};
  size_t env_size = 0;
  auto error = _wgetenv_s(&env_size, cmd_path, L"ComSpec");
  if (error) {
    ulg.error("wgetenv_s failed: {}", error);
    return {};
  }

  // command line

  const unsigned buf_size = std::max<unsigned>(64, (unsigned)name.size() + 16);
  std::unique_ptr<wchar_t[]> buf;

  try {
    buf = std::make_unique<wchar_t[]>(buf_size);
  } catch (const std::exception &e) {
    ulg.error("make_unique failed: {}", e.what());
    return {};
  }

  wchar_t *const buf_ptr = buf.get();
  _snwprintf_s(buf_ptr, buf_size, buf_size, L"/c \"where %s\"", name.c_str());

  int exit_code = utility::process::launchHiddenProgram(
      cmd_path, buf_ptr,
      [&path](std::string &&output) { path.append(output); });

  if (exit_code) {
    ulg.error("cmd exits with code {}", exit_code);
    path.clear();
  }

  return ansiToUnicode(path);
}

#elif defined(__linux__)

int launchHiddenProgram(const Str &path, Char *arg, RdtCbFunc func,
                        std::promise<bool> *p) {
  // TODO: split args?
  int pipefd[2];
  pid_t pid = 0;

  // create pipe
  if (pipe(pipefd) == -1) {
    ulg.error("launchHiddenProgram: %s", strerror("pipe"));
    return 1;
  }

  // create subprocess
  pid = fork();
  if (pid == -1) {
    ulg.error("launchHiddenProgram: %s", strerror("fork"));
    return 1;
  }

  // subprocess
  if (pid == 0) {
    close(pipefd[0]); // close reading end

    dup2(pipefd[1], STDOUT_FILENO); // redirect STDOUT to pipefd[0]
    dup2(pipefd[1], STDERR_FILENO);

    const char *args[] =
        //  {"ls", "-l", "-h", "-a", nullptr};
        {"./which", "ffmpeg", nullptr};

    execvp(path.c_str(), (char *const *)arg);

    // error occurred if it returns
    ulg.error("launchHiddenProgram: %s", strerror("execvp"));
    return 1;
  }

  // parent process

  ulg.debug("process {} launched", (int)pid);
  if (p)
    p->set_value(true);

  close(pipefd[1]); // close writing end

  // read output from pipe

  char buf[128];
  ssize_t numRead;
  while ((numRead = read(pipefd[0], buf, sizeof(buf) - 1)) > 0) {
    auto output{utility::string::trim(std::string(buf, numRead))};

    if (output.empty())
      continue;

    func(std::move(output));
  }

  if (numRead == -1)
    ulg.error("launchHiddenProgram: %s", strerror("read"));

  // wait for exit status

  int status = 0, exit_code = 0;
  waitpid(pid, &status, 0); // waiting for subprocess

  if (WIFEXITED(status))
    exit_code = WEXITSTATUS(status); // normal exit
  else if (WIFSIGNALED(status)) {
    exit_code = WTERMSIG(status);
    ulg.error("launchHiddenProgram: child killed by signal. %d", exit_code);
  }

  close(pipefd[0]);

  return exit_code;
}

Str locateProgramLinux(const Str &name) {
  const char *argv[] = {"which", name.c_str(), nullptr};
  std::string path;

  int exit_code = utility::process::launchHiddenProgram(
      argv[0], argv, [&path](std::string &&output) { path.append(output); });

  if (exit_code) {
    ulg.error("cmd exits with code {}", exit_code);
    path.clear();
  }

  return path;
}
#endif
} // namespace process
} // namespace utility
