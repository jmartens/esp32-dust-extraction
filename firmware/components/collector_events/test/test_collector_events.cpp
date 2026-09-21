#include <chrono>
#include <type_traits>

#include "unity.h"

#include "open_extraction/collector_events/collector_started.hpp"
#include "open_extraction/collector_events/collector_stopped.hpp"

namespace {

using open_extraction::event::DeviceUid;
using open_extraction::event::EventId;
using open_extraction::event::Timestamp;

template <typename CollectorEvent>
void assert_metadata(const CollectorEvent &event, const char *event_type) {
  TEST_ASSERT_EQUAL_STRING("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742",
                           event.event_id().data());
  TEST_ASSERT_EQUAL_STRING(event_type, event.event_type().data());
  TEST_ASSERT_EQUAL_INT64(1'726'258'400'123,
                          event.occurred_at().time_since_epoch().count());
  TEST_ASSERT_EQUAL_STRING("CONTROLLER-58BF25A77109",
                           event.originating_device().data());
}

EventId event_id() {
  return EventId::from_string("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742").value();
}

DeviceUid device_uid() {
  return DeviceUid::from_string("CONTROLLER-58BF25A77109").value();
}

Timestamp occurred_at() {
  return Timestamp(std::chrono::milliseconds(1'726'258'400'123));
}

} // namespace

TEST_CASE("a collector started event preserves immutable metadata",
          "[collector_events]") {
  const open_extraction::collector_events::CollectorStarted event(
      event_id(), occurred_at(), device_uid());

  assert_metadata(event, "collector.started");
  TEST_ASSERT_EQUAL_STRING(
      "collector.started",
      open_extraction::collector_events::CollectorStarted::kEventType.data());
  TEST_ASSERT_FALSE((std::is_copy_assignable_v<
                     open_extraction::collector_events::CollectorStarted>));
  TEST_ASSERT_FALSE((std::is_move_assignable_v<
                     open_extraction::collector_events::CollectorStarted>));
}

TEST_CASE("a collector stopped event preserves immutable metadata",
          "[collector_events]") {
  const open_extraction::collector_events::CollectorStopped event(
      event_id(), occurred_at(), device_uid());

  assert_metadata(event, "collector.stopped");
  TEST_ASSERT_EQUAL_STRING(
      "collector.stopped",
      open_extraction::collector_events::CollectorStopped::kEventType.data());
  TEST_ASSERT_FALSE((std::is_copy_assignable_v<
                     open_extraction::collector_events::CollectorStopped>));
  TEST_ASSERT_FALSE((std::is_move_assignable_v<
                     open_extraction::collector_events::CollectorStopped>));
}
