#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "open_extraction/event/event.hpp"

namespace open_extraction::event_dispatcher {

using SubscriptionId = std::uint32_t;
using EventHandler = std::function<void(const event::Event &)>;

class EventDispatcher {
public:
  [[nodiscard]] SubscriptionId subscribe(const event::EventType &event_type,
                                         EventHandler handler);
  [[nodiscard]] bool unsubscribe(SubscriptionId subscription_id);
  void publish(const event::Event &event);

private:
  struct Subscription {
    SubscriptionId id;
    std::string event_type;
    EventHandler handler;
  };

  SubscriptionId next_subscription_id_ = 1;
  std::vector<Subscription> subscriptions_;
};

} // namespace open_extraction::event_dispatcher
