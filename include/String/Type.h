#ifndef UTILITY_STRING_TYPE_H
#define UTILITY_STRING_TYPE_H

#include <memory>
#include <sstream>
#include <string>

namespace utility {
namespace string {
#if defined(_WIN32)
#define TXT(quote) L##quote
using Char = wchar_t;
using Sstream = std::wstringstream;
using Str = std::wstring;
#elif defined(__linux__)
#define TXT(quote) quote
using Char = char;
using Sstream = std::stringstream;
using Str = std::string;
#endif

using BufPtr = std::unique_ptr<Char[]>;
} // namespace string
} // namespace utility

#endif // UTILITY_STRING_TYPE_H
