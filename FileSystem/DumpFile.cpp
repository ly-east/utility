#include "Utility/FileSystem/DumpFile.h"
#include "ulog/ulog.h"
#include <cassert>

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
} // namespace filesystem
} // namespace utility
