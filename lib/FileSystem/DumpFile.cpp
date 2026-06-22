#include "Utility/FileSystem/DumpFile.h"
#include "ulog/ulog.h"
#include <cassert>
#include <filesystem>

namespace utility {
namespace filesystem {
bool DumpFile(const std::string &file_name, const std::string &content,
              const std::ios_base::openmode mode) {
  assert(!(mode & std::ios_base::in));

  std::ofstream file{file_name, mode};
  if (!file.is_open()) {
    ulg.error("failed to write {}", file_name);
    return false;
  }

  file << content;
  file.flush();
  file.close();

  return true;
}

bool CentralizedDump(const std::string &folder, const std::string &filename,
                     const std::string &content) {
  // create folder if it doesn't exist
  auto path = std::filesystem::current_path().append(folder);
  if (!std::filesystem::exists(path)) {
    if (!std::filesystem::create_directory(path)) {
      ulg.error("CentralizedDump: failed to create directory {}",
                path.string());
      return false;
    }
  } else if (!std::filesystem::is_directory(path)) {
    ulg.error("CentralizedDump: invalid output path");
    return false;
  }

  path.append(filename);
  return DumpFile(path.string(), content);
}
} // namespace filesystem
} // namespace utility
