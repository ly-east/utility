#ifndef UTILITY_EVENTBUS_EVENTBUS_H
#define UTILITY_EVENTBUS_EVENTBUS_H

#include "Utility/DllExport.h"
#include "Utility/EventBus/EventBase.h"
#include <functional>
#include <memory>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace utility {
namespace eventbus {
class DllExport EventBus {
public:
  using EventHandler = std::function<void(const EventBase &)>;
  using Token = std::shared_ptr<void>;

  static EventBus &Instance();

  EventBus(const EventBus &) = delete;
  EventBus(EventBus &&) = delete;

  EventBus &operator=(const EventBus &) = delete;
  EventBus &operator=(EventBus &&) = delete;

public:
  template <typename EventType> Token Subscribe(EventHandler handler);
  void Unsubscribe(const Token &token);

  template <typename EventType> void Publish(const EventType &event);

private:
  EventBus() = default;

private:
  // map: event type -> list of (handler, token)
  std::unordered_map<std::type_index,
                     std::vector<std::pair<EventHandler, std::weak_ptr<void>>>>
      map;
  std::mutex map_mutex;

  std::unordered_map<Token, std::type_index> token_map;
  std::mutex token_mutex;
};

template <typename EventType>
EventBus::Token EventBus::Subscribe(EventBus::EventHandler handler) {
  // Token to manage subscription lifetime
  auto token = std::make_shared<int>(0);
  const std::type_index type = typeid(EventType);

  {
    std::lock_guard<std::mutex> lock{token_mutex};
    token_map.emplace(token, type);
  }
  {
    std::lock_guard<std::mutex> lock{map_mutex};
    auto &subscribers = map[type];
    subscribers.emplace_back(handler, token);
  }

  return token;
}

template <typename EventType> void EventBus::Publish(const EventType &event) {
  std::lock_guard<std::mutex> lock{map_mutex};

  auto it = map.find(typeid(EventType));
  if (it != map.end()) {
    for (const auto &pair : it->second) {
      if (auto ptr = pair.second.lock())
        pair.first(event);
    }
  }
}
} // namespace eventbus
} // namespace utility

#endif // UTILITY_EVENTBUS_EVENTBUS_H
