#include "Utility/EventBus/EventBus.h"
#include <algorithm>

namespace utility {
namespace eventbus {
EventBus &EventBus::Instance() {
  static EventBus instance;
  return instance;
}

void EventBus::Unsubscribe(const Token &token) {
  std::lock_guard<std::mutex> lock{map_mutex};

  for (auto &[type, subscribers] : map) {
    auto it = std::remove_if(
        subscribers.begin(), subscribers.end(),
        [&token](const auto &pair) { return pair.second.lock() == token; });
    subscribers.erase(it, subscribers.end());
  }
}
} // namespace eventbus
} // namespace utility
