#include "Utility/EventBus/EventBus.h"
#include "ulog/ulog.h"
#include <algorithm>

namespace utility {
namespace eventbus {
EventBus &EventBus::Instance() {
  static EventBus instance;
  return instance;
}

void EventBus::Unsubscribe(const Token &token) {
  std::type_index type = typeid(void);

  {
    std::lock_guard<std::mutex> lock{token_mutex};
    auto iter = token_map.find(token);
    if (iter == token_map.end()) {
      ulg.warn("Unsubscribe: invalid token");
      return;
    }

    type = iter->second;
    token_map.erase(iter);
  }

  std::lock_guard<std::mutex> lock{map_mutex};

  auto &subscribers = map[type];
  auto it = std::remove_if(
      subscribers.begin(), subscribers.end(),
      [&token](const auto &pair) { return pair.second.lock() == token; });
  subscribers.erase(it, subscribers.end());
}
} // namespace eventbus
} // namespace utility
