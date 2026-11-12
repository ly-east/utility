#include "Utility/Time/UnixTimestamp.h"
#include <chrono>

namespace utility {
namespace time {
int64_t getUnixTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto seconds =
      std::chrono::duration_cast<std::chrono::seconds>(duration).count();
  return seconds;
}
} // namespace time
} // namespace utility
