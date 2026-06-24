#ifndef UTILITY_JSON_ARIASTATUS_H
#define UTILITY_JSON_ARIASTATUS_H

#include <filesystem>

namespace utility {
namespace aria {
enum class TaskStatus : unsigned {
  UNKNOWN,
  ACTIVE,
  COMPLETE,
  PAUSED,
  WAITING,
  FAILED
};

struct StatusResult {
  double progress;
  double speed; // KBps
  std::filesystem::path path;
  TaskStatus status;
};
} // namespace aria
} // namespace utility

#endif // UTILITY_JSON_ARIASTATUS_H
