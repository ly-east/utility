#include "Utility/String/Match.h"
#include <regex>

namespace {
static bool isMatched(const std::string &url, std::sregex_iterator &begin,
                      const char *pattern) {
  std::regex matcher{pattern};
  begin = std::sregex_iterator(url.begin(), url.end(), matcher);
  return std::distance(begin, std::sregex_iterator());
}
} // namespace

namespace utility {
namespace string {
bool isMatched(const std::string &str, const char *const pattern) {
  std::sregex_iterator it;
  return ::isMatched(str, it, pattern);
}

std::string getMatchedString(const std::string &str,
                             const char *const pattern) {
  std::sregex_iterator it;
  if (::isMatched(str, it, pattern))
    return std::smatch(*it).str();
  return {};
}

std::string getQueryString(const std::string &name, const std::string &url) {
  static const std::regex queryRegex{R"((^|&)?(\w+)=([^&]+)(&|$)?)"};
  std::smatch match;
  std::string::const_iterator searchStart{url.cbegin()};

  while (std::regex_search(searchStart, url.cend(), match, queryRegex)) {
    // match[2] and match[3] only exist when the regex actually captured at
    // least 3 groups.
    // match.size() is the number of submatches including the full match.
    // If the regex changed or a malformed match occurs, match[2] / match[3] may
    // be invalid.
    // The >= 4 guard avoids undefined behavior before accessing those groups.
    if (match.size() >= 4 && match[2] == name)
      return match[3].str();

    searchStart = match.suffix().first;
  }

  return {};
}
} // namespace string
} // namespace utility
