#include "Utility/String/Encoding.h"
#include <Windows.h>
#include <memory>

namespace utility {
namespace string {
std::string utf8ToString(const std::string &str) {
  int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
  ++nwLen; // to truncate string

  std::unique_ptr<wchar_t[]> wide_buf{std::make_unique<wchar_t[]>(nwLen)};
  wchar_t *pwBuf = wide_buf.get();
  memset(pwBuf, 0, nwLen * 2 + 2);
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

  int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
  ++nLen;

  std::unique_ptr<char[]> buf{std::make_unique<char[]>(nLen)};
  char *pBuf = buf.get();
  memset(pBuf, 0, nLen + 1);
  WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

  auto string{std::string(pBuf, strnlen(pBuf, nLen))};
  return string;
}

std::string stringToUtf8(const std::string &str) {
  int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
  ++nwLen; // to truncate string

  std::unique_ptr<wchar_t[]> wide_buf{std::make_unique<wchar_t[]>(nwLen)};
  wchar_t *pwBuf = wide_buf.get();
  ZeroMemory(pwBuf, nwLen * 2 + 2);
  MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

  int nLen = WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
  ++nLen;

  std::unique_ptr<char[]> buf{std::make_unique<char[]>(nLen)};
  char *pBuf = buf.get();
  ZeroMemory(pBuf, nLen + 1);
  WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

  auto string{std::string(pBuf, strnlen(pBuf, nLen))};
  return string;
}
} // namespace string
} // namespace utility
