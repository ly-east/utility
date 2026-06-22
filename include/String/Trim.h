#ifndef UTILITY_STRING_TRIM_H
#define UTILITY_STRING_TRIM_H

#include "DllExport.h"
#include <string>

namespace utility {
namespace string {
DllExport std::string leftTrim(const std::string &s);
DllExport std::string rightTrim(const std::string &s);
DllExport std::string trim(const std::string &s);
} // namespace string
} // namespace utility

#endif // UTILITY_STRING_TRIM_H
