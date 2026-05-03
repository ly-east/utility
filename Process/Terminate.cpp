#include "Utility/Process/Terminate.h"
#include "Utility/Process/Launch.h"
#include "Utility/String/Encoding.h"
#include "ulog/ulog.h"
#include <memory>

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
bool terminateProcessLinux(const utility::string::Str &name) { return false; }
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
