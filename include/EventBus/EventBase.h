#ifndef UTILITY_EVENTBUS_EVENTBASE_H
#define UTILITY_EVENTBUS_EVENTBASE_H

#include "DllExport.h"

namespace utility {
namespace eventbus {
struct DllExport EventBase {
  virtual ~EventBase() = default;
};
} // namespace eventbus
} // namespace utility

#endif // UTILITY_EVENTBUS_EVENTBASE_H
