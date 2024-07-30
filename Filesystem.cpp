#include "Utility/Filesystem.h"
#include "Utility/Process/Launch.h"
#include "spdlog/spdlog.h"
#include <filesystem>

namespace utility {
void createDirectory(const std::string &path) {
  const std::filesystem::path p{path};

  if (!std::filesystem::exists(p))
    std::filesystem::create_directories(p);
}

bool removePath(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    spdlog::warn("cannot remove a none-existed path {}", path.string());
    return false;
  }

  return std::filesystem::remove(path);
}
} // namespace utility
