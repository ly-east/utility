#ifndef UTILITY_STRING_MATCH_H
#define UTILITY_STRING_MATCH_H

#include "DllExport.h"
#include <string>

namespace utility {
namespace string {
DllExport bool isMatched(const std::string &str, const char *const pattern);

DllExport std::string getMatchedString(const std::string &str,
                                       const char *const pattern);
} // namespace string
} // namespace utility

#endif
