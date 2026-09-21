#include <chrono>
#include <utility>

#include "unity.h"

#include "open_extraction/event/event.hpp"

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

EventMetadata valid_metadata() {
    const auto event_id =
        EventId::from_string("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742").value();
    const auto event_type = EventType::from_string("collector.started").value();
    const auto device_uid =
        DeviceUid::from_string("CONTROLLER-58BF25A77109").value();

    return {
        .id = event_id,
        .type = event_type,
        .occurred_at = Timestamp(std::chrono::milliseconds(1'726'258'400'123)),
        .originating_device = device_uid,
    };
}

}  // namespace

TEST_CASE("an event exposes immutable metadata", "[event]") {
    const TestEvent event(valid_metadata());

    TEST_ASSERT_EQUAL_STRING(
        "ea290ba8-5255-4c7f-9fc6-9a1b94d5a742",
        event.event_id().data());
    TEST_ASSERT_EQUAL_STRING("collector.started", event.event_type().data());
    TEST_ASSERT_EQUAL_INT64(
        1'726'258'400'123,
        event.occurred_at().time_since_epoch().count());
    TEST_ASSERT_EQUAL_STRING(
        "CONTROLLER-58BF25A77109",
        event.originating_device().data());
}

TEST_CASE("event metadata rejects malformed identifiers", "[event]") {
    TEST_ASSERT_FALSE(EventId::from_string("not-a-uuid").has_value());
    TEST_ASSERT_FALSE(EventType::from_string("").has_value());
    TEST_ASSERT_FALSE(DeviceUid::from_string("controller-58BF25A77109").has_value());
}
