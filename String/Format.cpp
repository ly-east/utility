#include "Utility/String/Format.h"
#include "ulog/ulog.h"
#include <iomanip>
#include <sstream>

namespace utility {
namespace string {
std::string binToHex(const unsigned char *data, size_t len) {
  if (!data || !len) {
    ulg.error("binToHex: invalid parameter");
    return std::string();
  }

  std::ostringstream ss;
  ss << std::hex << std::setfill('0');

  for (size_t i = 0; i < len; ++i)
    ss << std::setw(2) << static_cast<int>(data[i]);

  return ss.str();
}
} // namespace string
} // namespace utility
