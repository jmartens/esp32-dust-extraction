#include <chrono>
#include <string>
#include <utility>
#include <vector>

#include "unity.h"

#include "open_extraction/event/event.hpp"
#include "open_extraction/event_dispatcher/event_dispatcher.hpp"

namespace {

using open_extraction::event::DeviceUid;
using open_extraction::event::Event;
using open_extraction::event::EventId;
using open_extraction::event::EventMetadata;
using open_extraction::event::EventType;
using open_extraction::event::Timestamp;
using open_extraction::event_dispatcher::EventDispatcher;

class TestEvent final : public Event {
public:
  explicit TestEvent(EventMetadata metadata) : Event(std::move(metadata)) {}
};

EventType event_type(std::string value) {
  return EventType::from_string(std::move(value)).value();
}

TestEvent create_event(const EventType &type) {
  return TestEvent({
      .id =
          EventId::from_string("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742").value(),
      .type = type,
      .occurred_at = Timestamp(std::chrono::milliseconds(1'726'258'400'123)),
      .originating_device =
          DeviceUid::from_string("CONTROLLER-58BF25A77109").value(),
  });
}

} // namespace

TEST_CASE("publishing with no subscribers does nothing", "[event_dispatcher]") {
  EventDispatcher dispatcher;
  const auto event = create_event(event_type("collector.started"));

  dispatcher.publish(event);
}

TEST_CASE("dispatches subscribers in registration order",
          "[event_dispatcher]") {
  EventDispatcher dispatcher;
  std::vector<int> calls;
  const auto type = event_type("collector.started");

  dispatcher.subscribe(type, [&calls](const Event &) { calls.push_back(1); });
  dispatcher.subscribe(type, [&calls](const Event &) { calls.push_back(2); });
  dispatcher.publish(create_event(type));

  TEST_ASSERT_EQUAL_UINT32(2, calls.size());
  TEST_ASSERT_EQUAL_INT(1, calls[0]);
  TEST_ASSERT_EQUAL_INT(2, calls[1]);
}

TEST_CASE("dispatches only subscribers for the published event type",
          "[event_dispatcher]") {
  EventDispatcher dispatcher;
  int started_calls = 0;
  int stopped_calls = 0;
  const auto started = event_type("collector.started");
  const auto stopped = event_type("collector.stopped");

  dispatcher.subscribe(started,
                       [&started_calls](const Event &) { ++started_calls; });
  dispatcher.subscribe(stopped,
                       [&stopped_calls](const Event &) { ++stopped_calls; });
  dispatcher.publish(create_event(started));

  TEST_ASSERT_EQUAL_INT(1, started_calls);
  TEST_ASSERT_EQUAL_INT(0, stopped_calls);
}

TEST_CASE("unsubscribing removes only the requested subscriber",
          "[event_dispatcher]") {
  EventDispatcher dispatcher;
  std::vector<int> calls;
  const auto type = event_type("collector.started");

  const auto first = dispatcher.subscribe(
      type, [&calls](const Event &) { calls.push_back(1); });
  dispatcher.subscribe(type, [&calls](const Event &) { calls.push_back(2); });

  TEST_ASSERT_TRUE(dispatcher.unsubscribe(first));
  TEST_ASSERT_FALSE(dispatcher.unsubscribe(first));

  dispatcher.publish(create_event(type));

  TEST_ASSERT_EQUAL_UINT32(1, calls.size());
  TEST_ASSERT_EQUAL_INT(2, calls[0]);
}

TEST_CASE("publishing an event with no matching subscriber is ignored",
          "[event_dispatcher]") {
  EventDispatcher dispatcher;
  int calls = 0;

  dispatcher.subscribe(event_type("collector.started"),
                       [&calls](const Event &) { ++calls; });
  dispatcher.publish(create_event(event_type("collector.faulted")));

  TEST_ASSERT_EQUAL_INT(0, calls);
}
