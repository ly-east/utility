#include "Utility/String/Trim.h"

namespace {
// all types of possible spaces to remove
static std::string toRemove = " \n\r\t\f\v";
} // namespace

namespace utility {
namespace string {
std::string leftTrim(const std::string &s) {
  // finding the index just after white spaces
  auto start = s.find_first_not_of(toRemove);

  // removed leading white spaces
  return start == std::string::npos ? "" : s.substr(start);
}

std::string rightTrim(const std::string &s) {
  // finding the index just before white spaces
  auto end = s.find_last_not_of(toRemove);

  // removed trailing white spaces
  return end == std::string::npos ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
  // first trimming the left white space i.e. leading white spaces and next
  // trimming the right white spaces i.e. trailing white spaces
  return rightTrim(leftTrim(s));
}
} // namespace string
} // namespace utility
