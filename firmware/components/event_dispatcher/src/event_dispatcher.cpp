#include "open_extraction/event_dispatcher/event_dispatcher.hpp"

#include <algorithm>
#include <utility>

namespace open_extraction::event_dispatcher {

SubscriptionId EventDispatcher::subscribe(const event::EventType &event_type,
                                          EventHandler handler) {
  const SubscriptionId subscription_id = next_subscription_id_++;
  subscriptions_.push_back(
      {subscription_id, std::string(event_type.value()), std::move(handler)});
  return subscription_id;
}

bool EventDispatcher::unsubscribe(SubscriptionId subscription_id) {
  const auto subscription =
      std::find_if(subscriptions_.begin(), subscriptions_.end(),
                   [subscription_id](const Subscription &candidate) {
                     return candidate.id == subscription_id;
                   });

  if (subscription == subscriptions_.end()) {
    return false;
  }

  subscriptions_.erase(subscription);
  return true;
}

void EventDispatcher::publish(const event::Event &event) {
  const std::string_view event_type = event.event_type();
  std::vector<EventHandler> subscribers;
  for (const Subscription &subscription : subscriptions_) {
    if (subscription.event_type == event_type) {
      subscribers.push_back(subscription.handler);
    }
  }

  for (const EventHandler &subscriber : subscribers) {
    subscriber(event);
  }
}

} // namespace open_extraction::event_dispatcher
