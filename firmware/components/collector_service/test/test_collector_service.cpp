#include <array>
#include <chrono>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "unity.h"

#include "open_extraction/collector_commands/start_collector.hpp"
#include "open_extraction/collector_commands/stop_collector.hpp"
#include "open_extraction/collector_events/collector_fault.hpp"
#include "open_extraction/collector_service/collector_service.hpp"

namespace {

using open_extraction::collector_commands::StartCollector;
using open_extraction::collector_commands::StopCollector;
using open_extraction::collector_events::CollectorFault;
using open_extraction::collector_service::CollectorService;
using open_extraction::collector_unit::CollectorCapability;
using open_extraction::collector_unit::CollectorDiagnostics;
using open_extraction::collector_unit::CollectorState;
using open_extraction::collector_unit::ICollectorUnit;
using open_extraction::command::Command;
using open_extraction::command_dispatcher::CommandDispatcher;
using open_extraction::event::DeviceUid;
using open_extraction::event::EventId;
using open_extraction::event::EventIdGenerator;
using open_extraction::event::EventType;
using open_extraction::event::Timestamp;
using open_extraction::event_dispatcher::EventDispatcher;

constexpr std::string_view kUid = "COLLECTOR-58BF25A77109";
constexpr std::string_view kEventId = "ea290ba8-5255-4c7f-9fc6-9a1b94d5a742";
const Timestamp kNow{std::chrono::milliseconds(1'726'258'400'123)};

class FakeUnit final : public ICollectorUnit {
public:
  CollectorState current = CollectorState::stopped;
  int starts = 0;
  int stops = 0;

  bool start() override {
    ++starts;
    return true;
  }
  bool stop() override {
    ++stops;
    return true;
  }
  CollectorState state() const override { return current; }
  CollectorDiagnostics diagnostics() const override { return {}; }
  std::span<const CollectorCapability> capabilities() const override {
    return {};
  }
};

class FixedIdGenerator final : public EventIdGenerator {
public:
  EventId generate() override {
    return EventId::from_string(std::string(kEventId)).value();
  }
};

struct Fixture {
  CommandDispatcher dispatcher;
  EventDispatcher event_dispatcher;
  FakeUnit unit;
  FixedIdGenerator ids;
  std::vector<std::string> received_types;
  std::vector<std::string> received_targets;
  std::vector<std::string> received_ids;
  std::vector<std::string> received_fault_reasons;
  std::vector<std::string> received_fault_ids;
  std::vector<std::string> received_fault_devices;
  bool deliver = true;

  Fixture() {
    TEST_ASSERT_TRUE(dispatcher.register_recipient(
        DeviceUid::from_string(std::string(kUid)).value(),
        [this](const Command &command) {
          received_types.emplace_back(command.command_type());
          received_targets.emplace_back(command.target());
          received_ids.emplace_back(command.command_id());
          return deliver;
        }));
    const auto fault_type =
        EventType::from_string(std::string(CollectorFault::kEventType)).value();
    (void)event_dispatcher.subscribe(
        fault_type, [this](const open_extraction::event::Event &event) {
          const auto reason = event.fault_reason();
          if (reason.has_value()) {
            received_fault_reasons.emplace_back(*reason);
            received_fault_ids.emplace_back(event.event_id());
            received_fault_devices.emplace_back(event.originating_device());
          }
        });
  }

  CollectorService service() {
    return CollectorService(dispatcher, event_dispatcher, unit,
                            DeviceUid::from_string(std::string(kUid)).value(),
                            ids, [] { return kNow; });
  }
};

} // namespace

TEST_CASE("service reports device uid and unit state", "[collector_service]") {
  Fixture f;
  CollectorService service = f.service();

  TEST_ASSERT_EQUAL_STRING("COLLECTOR-58BF25A77109",
                           service.device_uid().c_str());
  for (const auto state : {CollectorState::stopped, CollectorState::starting,
                           CollectorState::running, CollectorState::stopping,
                           CollectorState::faulted}) {
    f.unit.current = state;
    TEST_ASSERT_EQUAL_INT(static_cast<int>(state),
                          static_cast<int>(service.state()));
  }
}

TEST_CASE("request_start dispatches StartCollector to the device",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service();

  TEST_ASSERT_TRUE(service.request_start());
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StartCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
  TEST_ASSERT_EQUAL_STRING("COLLECTOR-58BF25A77109",
                           f.received_targets[0].c_str());
  TEST_ASSERT_EQUAL_STRING(std::string(kEventId).c_str(),
                           f.received_ids[0].c_str());
}

TEST_CASE("request_stop dispatches StopCollector when running",
          "[collector_service]") {
  Fixture f;
  f.unit.current = CollectorState::running;
  CollectorService service = f.service();

  TEST_ASSERT_TRUE(service.request_stop());
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StopCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
}

TEST_CASE("requests never change collector state or call the unit",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service();

  TEST_ASSERT_TRUE(service.request_start());
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorState::stopped),
                        static_cast<int>(service.state()));
  TEST_ASSERT_EQUAL_INT(0, f.unit.starts);
  TEST_ASSERT_EQUAL_INT(0, f.unit.stops);
}

TEST_CASE("requests not permitted by the state are rejected undispatched",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service();

  for (const auto state : {CollectorState::starting, CollectorState::running,
                           CollectorState::stopping, CollectorState::faulted}) {
    f.unit.current = state;
    TEST_ASSERT_FALSE(service.request_start());
  }
  for (const auto state : {CollectorState::stopped, CollectorState::starting,
                           CollectorState::stopping, CollectorState::faulted}) {
    f.unit.current = state;
    TEST_ASSERT_FALSE(service.request_stop());
  }
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());
}

TEST_CASE("request fails when the command is not delivered",
          "[collector_service]") {
  Fixture f;
  f.deliver = false;
  CollectorService service = f.service();
  TEST_ASSERT_FALSE(service.request_start());

  CommandDispatcher empty;
  CollectorService unregistered(
      empty, f.event_dispatcher, f.unit,
      DeviceUid::from_string(std::string(kUid)).value(), f.ids,
      [] { return kNow; });
  TEST_ASSERT_FALSE(unregistered.request_start());
}

TEST_CASE("report_fault publishes an immutable fault event with its reason",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service();

  service.report_fault("motor overload");

  TEST_ASSERT_EQUAL_UINT(1, f.received_fault_reasons.size());
  TEST_ASSERT_EQUAL_STRING("motor overload",
                           f.received_fault_reasons[0].c_str());
  TEST_ASSERT_EQUAL_STRING(std::string(kEventId).c_str(),
                           f.received_fault_ids[0].c_str());
  TEST_ASSERT_EQUAL_STRING(std::string(kUid).c_str(),
                           f.received_fault_devices[0].c_str());
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorState::stopped),
                        static_cast<int>(service.state()));
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());
}
