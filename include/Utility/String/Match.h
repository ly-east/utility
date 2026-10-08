#ifndef UTILITY_STRING_MATCH_H
#define UTILITY_STRING_MATCH_H

#include "Utility/DllExport.h"
#include <string>

namespace utility {
namespace string {
DllExport bool isMatched(const std::string &str, const char *const pattern);

DllExport std::string getMatchedString(const std::string &str,
                                       const char *const pattern);

// Returns the value of the query parameter with the given name from the URL.
DllExport std::string getQueryString(const std::string &name,
                                     const std::string &url);

// Returns the value of the Cookie field with the given name from the Cookie
// string.
DllExport std::string getCookieString(const std::string &name,
                                      const std::string &cookie);
} // namespace string
} // namespace utility

#endif
