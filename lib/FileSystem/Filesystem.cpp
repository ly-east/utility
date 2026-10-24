#include "Utility/FileSystem/Filesystem.h"
#include "Utility/String/Encoding.h"
#include "ulog/ulog.h"
#include <exception>
#include <functional>
#include <memory>
#include <string>

#if defined(_WIN32)
#include <Windows.h>
#include <shellapi.h>
#elif defined(__linux__)
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace {
using RemoveFuncTy = std::function<bool(const std::filesystem::path &)>;
using namespace utility::filesystem;

bool removeBatch(const PathListTy &path, RemoveFuncTy func) {
  bool result = true;

  for (const auto &p : path)
    result &= func(p);

  return result;
}
} // namespace

namespace utility {
namespace filesystem {
void createDirectory(const std::filesystem::path &p) {
  if (!std::filesystem::exists(p))
    std::filesystem::create_directories(p);
}

bool remove(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    ulg.warn("path doesn't exist. {}", path.string());
    return false;
  }

  try {
    std::filesystem::remove(path);
  } catch (const std::exception &e) {
    ulg.error("remove: {}", e.what());
    return false;
  }

  return true;
}

bool remove(const PathListTy &path) {
  return removeBatch(path, [](const std::filesystem::path &p) {
    return utility::filesystem::remove(p);
  });
}

bool removeAll(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    ulg.warn("cannot remove a none-existed path {}", path.string());
    return false;
  }

  return std::filesystem::remove_all(path);
}

bool removeAll(const PathListTy &path) {
  return removeBatch(path, [](const std::filesystem::path &p) {
    return utility::filesystem::removeAll(p);
  });
}

bool select(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    ulg.error("select: path doesn't exist. {}",
#if defined(_WIN32)
              utility::string::unicodeToUTF8(path.wstring())
#elif defined(__linux__)
              path.string()
#endif
    );
    return false;
  }

#if defined(_WIN32)
  const wchar_t select[] = L" /select,";
  const std::u16string filePath{path.u16string()};
  const size_t buf_size = wcslen(select) + filePath.size() * 2 + 4;

  try {
    std::unique_ptr<wchar_t[]> buf = std::make_unique<wchar_t[]>(buf_size);
    wchar_t *const buf_ptr = buf.get();
    _snwprintf_s(buf_ptr, buf_size, buf_size, L"%s\"%s\"", select,
                 (wchar_t *)filePath.c_str());

    // MSDN document allows to cast the HINSTANCE return type to an INT_PTR.
    // https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutew
    auto retVal = (INT_PTR)ShellExecuteW(NULL, L"open", L"explorer.exe",
                                         buf_ptr, NULL, SW_SHOWDEFAULT);

    if (!retVal || retVal <= 32) {
      ulg.error("select: ShellExecuteW error with return value {}", retVal);
      return false;
    }
  } catch (const std::exception &e) {
    ulg.error("select: {}", e.what());
    return false;
  }

  return true;
#elif defined(__linux__)
  // xdg-open cannot highlight a file inside a directory, so open the
  // containing directory instead. Double-fork keeps the call asynchronous
  // like ShellExecuteW and leaves no zombie process behind.
  std::filesystem::path dir{path};
  if (!std::filesystem::is_directory(dir))
    dir = dir.parent_path();
  if (dir.empty())
    dir = ".";

  const std::string target{dir.string()};

  const pid_t pid = fork();
  if (pid == -1) {
    ulg.error("select: fork failed ({})", strerror(errno));
    return false;
  }

  if (pid == 0) {
    const pid_t child = fork();
    if (child == -1)
      _exit(1);
    if (child > 0)
      _exit(0);

    // grandchild: detach from the terminal and run xdg-open
    const int fd = open("/dev/null", O_RDWR);
    if (fd != -1) {
      dup2(fd, STDIN_FILENO);
      dup2(fd, STDOUT_FILENO);
      dup2(fd, STDERR_FILENO);
      if (fd > STDERR_FILENO)
        close(fd);
    }

    execlp("xdg-open", "xdg-open", target.c_str(), (char *)nullptr);
    _exit(127);
  }

  int status = 0;
  waitpid(pid, &status, 0); // the intermediate child exits immediately

  return true;
#else
  return false;
#endif
}
} // namespace filesystem
} // namespace utility
