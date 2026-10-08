#ifndef UTILITY_STRING_FORMAT_H
#define UTILITY_STRING_FORMAT_H

#include "Utility/DllExport.h"
#include <ctime>
#include <string>

namespace utility {
namespace string {
// Convert binary data to hex string
DllExport std::string binToHex(const unsigned char *data, size_t len);

// Returns Unix timestamp (seconds since epoch), or -1 on parse failure.
DllExport std::time_t httpDateToUnixTimestamp(const std::string &date);
} // namespace string
} // namespace utility

#endif // UTILITY_STRING_FORMAT_H
