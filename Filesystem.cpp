#include "Utility/Filesystem.h"
#include "spdlog/spdlog.h"

namespace utility {
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
  } catch (const std::exception &) {
    return false;
  }

  return true;
}

bool removeAll(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    spdlog::warn("cannot remove a none-existed path {}", path.string());
    return false;
  }

  return std::filesystem::remove_all(path);
}
} // namespace utility
