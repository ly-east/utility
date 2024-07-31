#include "Utility/String/Encoding.h"
#include "spdlog/spdlog.h"
#include <Windows.h>
#include <memory>

namespace {
std::string conversion(const std::string &str, UINT codepage_l,
                       UINT codepage_r) {
  int nwLen = MultiByteToWideChar(codepage_l, 0, str.c_str(), -1, NULL, 0);
  ++nwLen; // to truncate string

  std::unique_ptr<wchar_t[]> wide_buf{std::make_unique<wchar_t[]>(nwLen)};
  wchar_t *pwBuf = wide_buf.get();
  memset(pwBuf, 0, nwLen * 2 + 2);
  MultiByteToWideChar(codepage_l, 0, str.c_str(), str.length(), pwBuf, nwLen);

  int nLen =
      WideCharToMultiByte(codepage_r, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
  ++nLen;

  std::unique_ptr<char[]> buf{std::make_unique<char[]>(nLen)};
  char *pBuf = buf.get();
  memset(pBuf, 0, nLen);
  WideCharToMultiByte(codepage_r, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

  return std::string(pBuf, strnlen(pBuf, nLen));
}
} // namespace

namespace utility {
namespace string {
std::string utf8ToString(const std::string &str) {
  return conversion(str, CP_UTF8, CP_ACP);
}

std::string stringToUtf8(const std::string &str) {
  return conversion(str, CP_ACP, CP_UTF8);
}
} // namespace string
} // namespace utility
