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

bool removePath(const std::string &path) {
  const std::filesystem::path p{path};

  if (!std::filesystem::exists(p)) {
    spdlog::warn("cannot remove a none-existed path {}", path);
    return false;
  }

  return std::filesystem::remove(p);
}
} // namespace utility
