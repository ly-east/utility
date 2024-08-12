#include "Utility/String/Encoding.h"
#include "spdlog/spdlog.h"
#include <Windows.h>
#include <codecvt>
#include <locale>
#include <memory>

namespace utility {
namespace string {
std::string utf8ToANSI(const std::string &str) {
  return unicodeToANSI(utf8ToUnicode(str));
}

std::string ansiToUtf8(const std::string &str) {
  return unicodeToUTF8(ansiToUnicode(str));
}

std::string unicodeToUTF8(const std::wstring &wstr) {
  std::string ret;

  try {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> wcv;
    ret = wcv.to_bytes(wstr);
  } catch (const std::exception &e) {
    spdlog::error("unicodeToUTF8: {}", e.what());
  }

  return ret;
}

std::wstring utf8ToUnicode(const std::string &str) {
  std::wstring ret;

  try {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> wcv;
    ret = wcv.from_bytes(str);
  } catch (const std::exception &e) {
    spdlog::error("utf8ToUnicode: {}", e.what());
  }

  return ret;
}

std::string unicodeToANSI(const std::wstring &wstr) {
  std::string ret;
  std::mbstate_t state = {};
  const wchar_t *src = wstr.data();

  setlocale(LC_CTYPE, "");
  size_t len = std::wcsrtombs(nullptr, &src, 0, &state);

  if (static_cast<size_t>(-1) != len) {
    std::unique_ptr<char[]> buff(new char[len + 1]);
    len = std::wcsrtombs(buff.get(), &src, len, &state);

    if (static_cast<size_t>(-1) != len)
      ret.assign(buff.get(), len);
  }

  return ret;
}

std::wstring ansiToUnicode(const std::string &str) {
  std::wstring ret;
  std::mbstate_t state = {};
  const char *src = str.data();
  size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);

  if (static_cast<size_t>(-1) != len) {
    std::unique_ptr<wchar_t[]> buff(new wchar_t[len + 1]);
    len = std::mbsrtowcs(buff.get(), &src, len, &state);

    if (static_cast<size_t>(-1) != len)
      ret.assign(buff.get(), len);
  }

  return ret;
}
} // namespace string
} // namespace utility
