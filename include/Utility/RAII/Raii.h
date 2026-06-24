#ifndef UTILITY_RAII_RAII_H
#define UTILITY_RAII_RAII_H

#include <type_traits>

namespace utility {
namespace raii {
// Invoke `func` with current value and default value when constructing and
// destructing this object respectively
template <typename FuncTy, typename ArgTy> class DefaultRestore {
public:
  DefaultRestore(FuncTy &&func, ArgTy &&current, ArgTy &&default_val)
      : func{std::move(func)}, current_value{std::move(current)},
        default_value{std::move(default_val)} {
    static_assert(std::is_invocable_v<FuncTy, ArgTy>);
    func(current_value);
  }

  ~DefaultRestore() { func(default_value); }

private:
  FuncTy func;
  ArgTy current_value;
  ArgTy default_value;
};
} // namespace raii
} // namespace utility

#endif // UTILITY_RAII_RAII_H
