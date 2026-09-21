#include <chrono>
#include <utility>

#include "unity.h"

#include "open_extraction/event/event.hpp"
#include "open_extraction/event_json/event_json_codec.hpp"

namespace {

using open_extraction::event::DeviceUid;
using open_extraction::event::Event;
using open_extraction::event::EventId;
using open_extraction::event::EventMetadata;
using open_extraction::event::EventType;
using open_extraction::event::Timestamp;

class TestEvent final : public Event {
public:
  explicit TestEvent(EventMetadata metadata) : Event(std::move(metadata)) {}
};

TestEvent create_event() {
  return TestEvent({
      .id =
          EventId::from_string("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742").value(),
      .type = EventType::from_string("collector.started").value(),
      .occurred_at = Timestamp(std::chrono::milliseconds(1'726'258'400'123)),
      .originating_device =
          DeviceUid::from_string("CONTROLLER-58BF25A77109").value(),
  });
}

} // namespace

TEST_CASE("the event JSON codec serializes event metadata", "[event_json]") {
  const auto serialized = EventJsonCodec::serialize(create_event());

  TEST_ASSERT_TRUE(serialized.has_value());
  TEST_ASSERT_EQUAL_STRING(
      "{\"event_id\":\"ea290ba8-5255-4c7f-9fc6-9a1b94d5a742\","
      "\"event_type\":\"collector.started\","
      "\"occurred_at_unix_ms\":1726258400123,"
      "\"originating_device\":\"CONTROLLER-58BF25A77109\"}",
      serialized->c_str());
}
