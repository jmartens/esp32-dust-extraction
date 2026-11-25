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
using open_extraction::collector_unit::CollectorMode;
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
  Timestamp now = kNow;

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

  CollectorService service(std::chrono::milliseconds start_delay = {},
                           std::chrono::milliseconds run_on = {}) {
    return CollectorService(
        dispatcher, event_dispatcher, unit,
        DeviceUid::from_string(std::string(kUid)).value(), ids,
        [this] { return now; }, start_delay, run_on);
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

TEST_CASE("forced on starts the collector without machine demand",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service();

  TEST_ASSERT_TRUE(service.request_mode(CollectorMode::forced_on));

  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::forced_on),
                        static_cast<int>(service.mode()));
  TEST_ASSERT_FALSE(service.demand_active());
  TEST_ASSERT_FALSE(service.demand_suppressed());
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StartCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
}

TEST_CASE("failed override dispatch leaves the previous mode selected",
          "[collector_service]") {
  Fixture f;
  f.deliver = false;
  CollectorService service = f.service();

  TEST_ASSERT_FALSE(service.request_mode(CollectorMode::forced_on));

  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::automatic),
                        static_cast<int>(service.mode()));
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StartCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
}

TEST_CASE("forced on keeps the collector on when machine demand is active",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service(std::chrono::milliseconds(500));

  TEST_ASSERT_TRUE(service.update_demand(true));
  TEST_ASSERT_TRUE(service.request_mode(CollectorMode::forced_on));

  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::forced_on),
                        static_cast<int>(service.mode()));
  TEST_ASSERT_TRUE(service.demand_active());
  TEST_ASSERT_FALSE(service.demand_suppressed());
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StartCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
}

TEST_CASE("forced off stops output and exposes active demand as suppressed",
          "[collector_service]") {
  Fixture f;
  f.unit.current = CollectorState::running;
  CollectorService service = f.service();

  TEST_ASSERT_TRUE(service.update_demand(true));
  TEST_ASSERT_TRUE(service.request_mode(CollectorMode::forced_off));

  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::forced_off),
                        static_cast<int>(service.mode()));
  TEST_ASSERT_TRUE(service.demand_active());
  TEST_ASSERT_TRUE(service.demand_suppressed());
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StopCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
}

TEST_CASE("forced off suppresses an automatic start despite active demand",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service();

  TEST_ASSERT_TRUE(service.request_mode(CollectorMode::forced_off));
  TEST_ASSERT_TRUE(service.update_demand(true));

  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::forced_off),
                        static_cast<int>(service.mode()));
  TEST_ASSERT_TRUE(service.demand_active());
  TEST_ASSERT_TRUE(service.demand_suppressed());
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorState::stopped),
                        static_cast<int>(service.state()));
}

TEST_CASE("automatic mode delays only newly active demand until delay expires",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service(std::chrono::milliseconds(500));

  TEST_ASSERT_TRUE(service.update_demand(true));
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());
  TEST_ASSERT_TRUE(service.demand_active());
  TEST_ASSERT_FALSE(service.demand_suppressed());

  f.now += std::chrono::milliseconds(499);
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());

  f.now += std::chrono::milliseconds(1);
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StartCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
}

TEST_CASE("returning to automatic starts active demand without start delay",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service(std::chrono::milliseconds(1'000));

  TEST_ASSERT_TRUE(service.update_demand(true));
  TEST_ASSERT_TRUE(service.request_mode(CollectorMode::forced_off));
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());
  TEST_ASSERT_TRUE(service.demand_active());
  TEST_ASSERT_TRUE(service.demand_suppressed());

  TEST_ASSERT_TRUE(service.request_mode(CollectorMode::automatic));

  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::automatic),
                        static_cast<int>(service.mode()));
  TEST_ASSERT_TRUE(service.demand_active());
  TEST_ASSERT_FALSE(service.demand_suppressed());
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StartCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
}

TEST_CASE("released automatic demand cancels a pending delayed start",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service(std::chrono::milliseconds(500));

  TEST_ASSERT_TRUE(service.update_demand(true));
  TEST_ASSERT_TRUE(service.process());
  f.now += std::chrono::milliseconds(250);
  TEST_ASSERT_TRUE(service.update_demand(false));
  TEST_ASSERT_TRUE(service.process());
  f.now += std::chrono::milliseconds(500);
  TEST_ASSERT_TRUE(service.process());

  TEST_ASSERT_FALSE(service.demand_active());
  TEST_ASSERT_FALSE(service.demand_suppressed());
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());
}

TEST_CASE("zero start delay starts new automatic demand immediately",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service(std::chrono::milliseconds::zero());

  TEST_ASSERT_TRUE(service.update_demand(true));

  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StartCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
}

TEST_CASE("negative start delay is clamped to no delay",
          "[collector_service]") {
  Fixture f;
  CollectorService service = f.service(std::chrono::milliseconds(-250));

  TEST_ASSERT_TRUE(service.update_demand(true));

  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
}

TEST_CASE("run-on keeps the collector running until the period elapses",
          "[collector_service]") {
  Fixture f;
  f.unit.current = CollectorState::running;
  CollectorService service =
      f.service(std::chrono::milliseconds{}, std::chrono::milliseconds(10'000));

  TEST_ASSERT_TRUE(service.update_demand(true));
  TEST_ASSERT_TRUE(service.update_demand(false));
  f.now += std::chrono::milliseconds(9'999);
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());

  f.now += std::chrono::milliseconds(1);
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StopCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
}

TEST_CASE("new demand cancels a pending run-on stop", "[collector_service]") {
  Fixture f;
  f.unit.current = CollectorState::running;
  CollectorService service =
      f.service(std::chrono::milliseconds{}, std::chrono::milliseconds(10'000));

  TEST_ASSERT_TRUE(service.update_demand(true));
  TEST_ASSERT_TRUE(service.update_demand(false));
  f.now += std::chrono::milliseconds(5'000);
  TEST_ASSERT_TRUE(service.update_demand(true));
  f.now += std::chrono::milliseconds(60'000);
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());

  // The run-on period restarts from the next demand release.
  TEST_ASSERT_TRUE(service.update_demand(false));
  f.now += std::chrono::milliseconds(9'999);
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());
  f.now += std::chrono::milliseconds(1);
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
}

TEST_CASE("run-on starts only after the last demand is removed",
          "[collector_service]") {
  Fixture f;
  f.unit.current = CollectorState::running;
  CollectorService service =
      f.service(std::chrono::milliseconds{}, std::chrono::milliseconds(1'000));

  // The service sees aggregated demand: it stays active while any unit is on.
  TEST_ASSERT_TRUE(service.update_demand(true));
  TEST_ASSERT_TRUE(service.update_demand(true));
  f.now += std::chrono::milliseconds(5'000);
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());

  TEST_ASSERT_TRUE(service.update_demand(false));
  f.now += std::chrono::milliseconds(1'000);
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
}

TEST_CASE("zero run-on stops immediately when demand ends",
          "[collector_service]") {
  Fixture f;
  f.unit.current = CollectorState::running;
  CollectorService service = f.service();

  TEST_ASSERT_TRUE(service.update_demand(true));
  TEST_ASSERT_TRUE(service.update_demand(false));
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StopCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());
}

TEST_CASE("run-on does not apply to forced modes", "[collector_service]") {
  Fixture f;
  f.unit.current = CollectorState::running;
  CollectorService service =
      f.service(std::chrono::milliseconds{}, std::chrono::milliseconds(10'000));

  TEST_ASSERT_TRUE(service.update_demand(true));
  TEST_ASSERT_TRUE(service.request_mode(CollectorMode::forced_off));
  TEST_ASSERT_EQUAL_UINT(1, f.received_types.size());
  TEST_ASSERT_EQUAL_STRING(std::string(StopCollector::kCommandType).c_str(),
                           f.received_types[0].c_str());

  f.unit.current = CollectorState::running;
  f.received_types.clear();
  TEST_ASSERT_TRUE(service.request_mode(CollectorMode::forced_on));
  TEST_ASSERT_TRUE(service.update_demand(false));
  f.now += std::chrono::milliseconds(60'000);
  TEST_ASSERT_TRUE(service.process());
  TEST_ASSERT_EQUAL_UINT(0, f.received_types.size());
}
