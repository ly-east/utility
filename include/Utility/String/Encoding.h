#ifndef UTILITY_STRING_ENCODING_H
#define UTILITY_STRING_ENCODING_H

#include "Utility/DllExport.h"
#include <string>

namespace utility {
namespace string {
// Directions of string encoding conversion:
//
// UTF-8 <--> Unicode <--> ANSI
//

DllExport std::string utf8ToANSI(const std::string &str);

DllExport std::string ansiToUtf8(const std::string &str);

DllExport std::string unicodeToUTF8(const std::wstring &wstr);

DllExport std::wstring utf8ToUnicode(const std::string &str);

DllExport std::string unicodeToANSI(const std::wstring &wstr);

DllExport std::wstring ansiToUnicode(const std::string &str);

DllExport std::u16string utf8ToUtf16(const std::string &str);

DllExport std::string utf16ToUtf8(const std::u16string &str);
} // namespace string
} // namespace utility

#endif // UTILITY_STRING_ENCODING_H
