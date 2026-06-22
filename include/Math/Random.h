#ifndef UTILITY_MATH_RANDOM_H
#define UTILITY_MATH_RANDOM_H

#include "DllExport.h"
#include <cassert>
#include <chrono>
#include <random>
#include <type_traits>

namespace utility {
namespace math {
template <typename UIntTy>
DllExport UIntTy getRandom(UIntTy upper_bound, UIntTy lower_bound = 0) {
  static_assert(std::is_unsigned_v<UIntTy>, "Unsigned integral is required");
  assert(upper_bound > lower_bound);

  static std::default_random_engine generator;
  static auto last_seed_time = std::chrono::system_clock::time_point::min();

  auto now = std::chrono::system_clock::now();
  if (now - last_seed_time > std::chrono::milliseconds(100)) {
    auto seed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch())
                    .count();
    generator.seed(seed);
    last_seed_time = now;
  }

  std::uniform_int_distribution<UIntTy> distribution{lower_bound,
                                                     upper_bound - 1};
  return distribution(generator);
}
} // namespace math
} // namespace utility

#endif // UTILITY_MATH_RANDOM_H
