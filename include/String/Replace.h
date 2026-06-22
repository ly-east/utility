#ifndef UTILITY_STRING_REPLACE_H
#define UTILITY_STRING_REPLACE_H

#include "DllExport.h"
#include <string>
#include <type_traits>
#include <unordered_set>

namespace utility {
namespace string {
template <typename StrTy> DllExport StrTy replaceSpecialChar(const StrTy &str) {
  static_assert(std::is_same_v<StrTy, std::string> ||
                std::is_same_v<StrTy, std::wstring> ||
                std::is_same_v<StrTy, std::u16string>);

  static std::unordered_set<typename StrTy::value_type> table{
      '\\', '/', ':', '*', '?', '"', '<', '>', '.', '|'};

  StrTy dst_str = str;
  typename StrTy::size_type pos = 0;

  for (auto c : table) {
    while ((pos = dst_str.find(c)) != StrTy::npos)
      dst_str.erase(pos, 1);
  }

  return dst_str;
}
} // namespace string
} // namespace utility

#endif // UTILITY_STRING_REPLACE_H
