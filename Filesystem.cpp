#include "Utility/Filesystem.h"
#include "spdlog/spdlog.h"
#include <filesystem>

namespace utility {
void createDirectory(const std::string &path) {
  const std::filesystem::path p{path};

  if (!std::filesystem::exists(p))
    std::filesystem::create_directories(p);
}

std::string getExecutableFilePath(const std::string &name) {
  std::filesystem::path p{std::filesystem::current_path()};
  p.append(name);
  if (!p.has_extension())
    p.replace_extension(".exe");

  // Program at the same path of BBDown is preferred to use
  if (std::filesystem::exists(p) && std::filesystem::is_regular_file(p))
    return p.string();

  // TODO: use WHERE command to find program in environment variable
  //  cmd /c "where ffmpeg"
  std::string path;

  return path;
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
