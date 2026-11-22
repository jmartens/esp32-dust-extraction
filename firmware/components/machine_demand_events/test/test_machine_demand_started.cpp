#include <chrono>
#include <string>
#include <type_traits>
#include <vector>

#include "unity.h"

#include "open_extraction/event/event.hpp"
#include "open_extraction/event_json/event_json_codec.hpp"
#include "open_extraction/machine_demand_events/machine_demand_started.hpp"
#include "open_extraction/machine_demand_events/machine_demand_started_emitter.hpp"

namespace {

using open_extraction::event::DeviceUid;
using open_extraction::event::EventId;
using open_extraction::event::Timestamp;
using open_extraction::event_json::EventJsonCodec;
using open_extraction::machine_demand_events::MachineDemandStarted;
using open_extraction::machine_demand_events::MachineDemandStartedEmitter;
using open_extraction::machine_demand_threshold::DemandThresholdEvaluator;
using open_extraction::machine_demand_threshold::DemandTransition;
using open_extraction::machine_demand_threshold::MachineDemandThresholdProfile;
using open_extraction::machine_demand_threshold::MonotonicClock;

class TestIds final : public open_extraction::event::EventIdGenerator {
public:
  EventId generate() override {
    ++count;
    const char *value = "ea290ba8-5255-4c7f-9fc6-9a1b94d5a744";
    if (count == 1) {
      value = "ea290ba8-5255-4c7f-9fc6-9a1b94d5a742";
    } else if (count == 2) {
      value = "ea290ba8-5255-4c7f-9fc6-9a1b94d5a743";
    }
    return EventId::from_string(value).value();
  }
  int count = 0;
};

MachineDemandStarted create_event() {
  return MachineDemandStarted(
      EventId::from_string("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742").value(),
      Timestamp(std::chrono::milliseconds(1'726'258'400'123)),
      DeviceUid::from_string("CONTROLLER-58BF25A77109").value(), "SAW-01");
}

} // namespace

TEST_CASE("a machine demand started event identifies its originating unit",
          "[machine_demand_events]") {
  const auto event = create_event();

  TEST_ASSERT_EQUAL_STRING("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742",
                           event.event_id().data());
  TEST_ASSERT_EQUAL_STRING("machine_demand.started", event.event_type().data());
  TEST_ASSERT_EQUAL_INT64(1'726'258'400'123,
                          event.occurred_at().time_since_epoch().count());
  TEST_ASSERT_EQUAL_STRING("CONTROLLER-58BF25A77109",
                           event.originating_device().data());
  TEST_ASSERT_TRUE(event.unit_id().has_value());
  TEST_ASSERT_EQUAL_STRING("SAW-01", event.unit_id()->data());
  TEST_ASSERT_FALSE((std::is_copy_assignable_v<MachineDemandStarted>));
  TEST_ASSERT_FALSE((std::is_move_assignable_v<MachineDemandStarted>));
}

TEST_CASE("a machine demand started event serializes its unit identifier",
          "[machine_demand_events]") {
  const auto serialized = EventJsonCodec::serialize(create_event());

  TEST_ASSERT_TRUE(serialized.has_value());
  TEST_ASSERT_EQUAL_STRING(
      "{\"event_id\":\"ea290ba8-5255-4c7f-9fc6-9a1b94d5a742\","
      "\"event_type\":\"machine_demand.started\","
      "\"occurred_at_unix_ms\":1726258400123,"
      "\"originating_device\":\"CONTROLLER-58BF25A77109\","
      "\"unit_id\":\"SAW-01\"}",
      serialized->c_str());
}

TEST_CASE("started is published only on debounced inactive to active changes",
          "[machine_demand_events]") {
  using namespace std::chrono_literals;
  const MachineDemandThresholdProfile profile{
      .start_threshold_amps = 5.0F,
      .stop_threshold_amps = 2.0F,
      .debounce_duration = 100ms,
  };
  auto evaluator = DemandThresholdEvaluator::create(profile);
  TEST_ASSERT_TRUE(evaluator.has_value());
  open_extraction::event_dispatcher::EventDispatcher dispatcher;
  TestIds ids;
  int clock_calls = 0;
  std::vector<std::string> events;
  const auto subscription = dispatcher.subscribe(
      open_extraction::event::EventType::from_string("machine_demand.started")
          .value(),
      [&events](const open_extraction::event::Event &event) {
        const auto json = EventJsonCodec::serialize(event);
        TEST_ASSERT_TRUE(json.has_value());
        events.push_back(*json);
      });
  (void)subscription;
  MachineDemandStartedEmitter emitter(
      *evaluator, DeviceUid::from_string("CONTROLLER-58BF25A77109").value(),
      "SAW-01", ids,
      [&clock_calls] {
        ++clock_calls;
        return Timestamp(std::chrono::milliseconds(1'726'258'400'123));
      },
      dispatcher);
  const MonotonicClock::time_point start{};

  TEST_ASSERT_TRUE(emitter.evaluate(6.0F, start) == DemandTransition::none);
  TEST_ASSERT_TRUE(emitter.evaluate(6.0F, start + 99ms) ==
                   DemandTransition::none);
  TEST_ASSERT_TRUE(events.empty());
  TEST_ASSERT_TRUE(emitter.evaluate(6.0F, start + 100ms) ==
                   DemandTransition::started);
  TEST_ASSERT_TRUE(emitter.demand_active());
  TEST_ASSERT_TRUE(emitter.evaluate(6.0F, start + 200ms) ==
                   DemandTransition::none);
  TEST_ASSERT_TRUE(emitter.evaluate(1.0F, start + 300ms) ==
                   DemandTransition::none);
  TEST_ASSERT_TRUE(emitter.evaluate(1.0F, start + 400ms) ==
                   DemandTransition::stopped);
  TEST_ASSERT_FALSE(emitter.demand_active());
  TEST_ASSERT_FALSE(emitter.evaluate(std::nullopt, start + 500ms).has_value());
  TEST_ASSERT_TRUE(emitter.evaluate(6.0F, start + 600ms) ==
                   DemandTransition::none);
  TEST_ASSERT_TRUE(emitter.evaluate(6.0F, start + 700ms) ==
                   DemandTransition::started);
  TEST_ASSERT_EQUAL_INT(3, ids.count);
  TEST_ASSERT_EQUAL_INT(3, clock_calls);
  TEST_ASSERT_EQUAL_UINT32(2, events.size());
  TEST_ASSERT_EQUAL_STRING(
      "{\"event_id\":\"ea290ba8-5255-4c7f-9fc6-9a1b94d5a742\","
      "\"event_type\":\"machine_demand.started\","
      "\"occurred_at_unix_ms\":1726258400123,"
      "\"originating_device\":\"CONTROLLER-58BF25A77109\","
      "\"unit_id\":\"SAW-01\"}",
      events[0].c_str());
  TEST_ASSERT_NOT_EQUAL(0, events[0].compare(events[1]));
}
