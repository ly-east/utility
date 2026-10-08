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

std::time_t httpDateToUnixTimestamp(const std::string &date) {
  std::tm tm{};
  std::istringstream in{s};
  in.imbue(std::locale::classic()); // month/weekday names are locale-sensitive
  in >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S");
  if (in.fail())
    return -1;

#ifdef _WIN32
  return _mkgmtime(&tm); // MSVC: interprets tm as UTC
#else
  return timegm(&tm); // POSIX equivalent
#endif
}
} // namespace string
} // namespace utility
