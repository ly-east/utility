#include "Utility/Process/Terminate.h"
#include "Utility/Process/Launch.h"
#include "Utility/String/Encoding.h"
#include "ulog/ulog.h"
#include <memory>

#if defined(__linux__)
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <signal.h>
#include <unistd.h>
#endif

namespace {
#if defined(_WIN32)
bool terminateProcessWindows(const utility::string::Str &name) {
  // locate taskkill

  wchar_t root_path[64]{};
  size_t env_size = 0;
  auto error = _wgetenv_s(&env_size, root_path, L"SystemRoot");
  if (error) {
    ulg.error("wgetenv_s failed: {}", error);
    return {};
  }

  constexpr const wchar_t *const program_suffix{L"\\system32\\taskkill.exe"};
  size_t program_length = wcsnlen(root_path, std::size(root_path)) +
                          wcsnlen(program_suffix, 32) + 16;
  std::unique_ptr<wchar_t[]> program = nullptr;

  try {
    program = std::make_unique<wchar_t[]>(program_length);
  } catch (const std::exception &e) {
    ulg.error(
        "terminateProcessWindows: failed to allocate memory for program {}",
        e.what());
    return false;
  }
  _snwprintf_s(program.get(), program_length, program_length, L"%s%s",
               root_path, program_suffix);

  // generate parameters

  constexpr const wchar_t *const cmdline{L"/F /IM \"%s\""};

  size_t length = wcsnlen(cmdline, 32) + name.length();
  std::unique_ptr<wchar_t[]> buffer = nullptr;

  try {
    buffer = std::make_unique<wchar_t[]>(length);
  } catch (const std::exception &e) {
    ulg.error(
        "terminateProcessWindows: failed to allocate memory for cmdline {}",
        e.what());
    return false;
  }
  _snwprintf_s(buffer.get(), length, length, cmdline, name.c_str());

  // run

  int exitcode = utility::process::launchHiddenProgram(
      program.get(), buffer.get(), [](std::string &&str) {});

  if (exitcode) {
    ulg.error("taskkill exits with code {}", exitcode);
    return false;
  }
  return true;
}
#elif defined(__linux__)
// taskkill /F /IM semantics: force-kill every process whose image name equals
// `name`. Candidates are resolved through /proc at kill time, so a stale or
// reused pid can never be targeted.
bool terminateProcessLinux(const utility::string::Str &name) {
  namespace fs = std::filesystem;

  std::error_code ec;
  fs::directory_iterator it{"/proc", ec};
  const fs::directory_iterator end;
  if (ec) {
    ulg.error("terminateProcessLinux: cannot open /proc ({})", ec.message());
    return false;
  }

  bool killed = false;
  const pid_t self = getpid();

  for (; it != end; it.increment(ec)) {
    const std::string entry{it->path().filename().string()};
    if (entry.empty() || !std::isdigit((unsigned char)entry.front()))
      continue;

    const pid_t pid = (pid_t)std::strtol(entry.c_str(), nullptr, 10);
    if (pid <= 0 || pid == self)
      continue;

    // match the image name by the executable basename, falling back to
    // /proc/<pid>/comm (which the kernel truncates to 15 characters)
    std::error_code exe_ec;
    const fs::path exe{fs::read_symlink(it->path() / "exe", exe_ec)};
    if (exe_ec || exe.filename() != name) {
      std::ifstream comm_file{it->path() / "comm"};
      std::string comm;
      std::getline(comm_file, comm);
      if (comm != name)
        continue;
    }

    if (kill(pid, SIGKILL) == 0) {
      ulg.debug("terminateProcessLinux: killed {} ({})", name, pid);
      killed = true;
    } else if (ESRCH != errno) {
      ulg.error("terminateProcessLinux: kill {} failed ({})", pid,
                strerror(errno));
    }
  }

  if (!killed)
    ulg.warn("terminateProcessLinux: no process named {} found", name);

  return killed;
}
#endif
} // namespace

namespace utility {
namespace process {
bool terminateProcess(const utility::string::Str &name) {
#if defined(_WIN32)
  return terminateProcessWindows(name);
#elif defined(__linux__)
  return terminateProcessLinux(name);
#endif
}
} // namespace process
} // namespace utility
