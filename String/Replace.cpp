#include "Utility/String/Replace.h"
#include <unordered_set>

namespace utility {
namespace string {
std::string replaceSpecialChar(const std::string &str) {
  static std::unordered_set<char> table{'\\', '/', ':', '*', '?',
                                        '"',  '<', '>', '.', '|'};

  std::string dst_str = str;
  std::string::size_type pos = 0;

  for (auto c : table) {
    while ((pos = dst_str.find(c)) != std::string::npos)
      dst_str.replace(pos, 1, "_");
  }

  return dst_str;
}
} // namespace string
} // namespace utility
