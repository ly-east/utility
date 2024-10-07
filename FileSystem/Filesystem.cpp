#include "Utility/FileSystem/Filesystem.h"
#include "spdlog/spdlog.h"
#include <functional>

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
    spdlog::warn("cannot remove a none-existed path {}", path.string());
    return false;
  }

  try {
    std::filesystem::remove(path);
  } catch (const std::exception &e) {
    spdlog::error("remove: {}", e.what());
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
    spdlog::warn("cannot remove a none-existed path {}", path.string());
    return false;
  }

  return std::filesystem::remove_all(path);
}

bool removeAll(const PathListTy &path) {
  return removeBatch(path, [](const std::filesystem::path &p) {
    return utility::filesystem::removeAll(p);
  });
}
} // namespace filesystem
} // namespace utility
