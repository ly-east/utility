#ifndef UTILITY_MATH_CORRECTVALUE_H
#define UTILITY_MATH_CORRECTVALUE_H

#include "Utility/DllExport.h"
#include <algorithm>
#include <cmath>

namespace utility {
namespace math {
constexpr double DEFAULT_EPS = 1e-12;

DllExport inline bool isEqual(const double a, const double b,
                              double eps = DEFAULT_EPS) {
  const double diff = std::abs(a - b);

  if (diff < eps)
    return true;

  return diff < eps * std::max(std::abs(a), std::abs(b));
}

DllExport inline bool isValid(const double value) {
  return std::isfinite(value);
}
} // namespace math
} // namespace utility

#endif // UTILITY_MATH_CORRECTVALUE_H
