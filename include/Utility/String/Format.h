#ifndef UTILITY_STRING_FORMAT_H
#define UTILITY_STRING_FORMAT_H

#include "Utility/DllExport.h"
#include <string>

namespace utility {
namespace string {
// Convert binary data to hex string
DllExport std::string binToHex(const unsigned char *data, size_t len);
} // namespace string
} // namespace utility

#endif // UTILITY_STRING_FORMAT_H
