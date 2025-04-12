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
} // namespace string
} // namespace utility
