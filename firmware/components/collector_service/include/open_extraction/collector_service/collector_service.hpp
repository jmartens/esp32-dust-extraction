#pragma once

#include <chrono>
#include <functional>
#include <optional>
#include <string>

#include "collector_unit/collector_unit.hpp"
#include "open_extraction/collector_api/collector_api.hpp"
#include "open_extraction/command_dispatcher/command_dispatcher.hpp"
#include "open_extraction/event/event.hpp"
#include "open_extraction/event_dispatcher/event_dispatcher.hpp"

namespace open_extraction::collector_service {

using Clock = std::function<event::Timestamp()>;

// Concrete ICollectorControl. Start and stop are issued as commands through
// the command dispatcher; the service never changes collector state itself.
// State changes only happen when the recipient handles the command and the
// resulting events are processed (ADR-0009, ADR-0011).
class CollectorService final : public collector_api::ICollectorControl {
public:
  // All references must outlive the service.
  CollectorService(command_dispatcher::CommandDispatcher &dispatcher,
                   event_dispatcher::EventDispatcher &event_dispatcher,
                   const collector_unit::ICollectorUnit &unit,
                   event::DeviceUid device_uid,
                   event::EventIdGenerator &id_generator, Clock clock,
                   std::chrono::milliseconds start_delay = {},
                   std::chrono::milliseconds run_on = {});

  [[nodiscard]] collector_unit::CollectorState state() const override;
  [[nodiscard]] collector_unit::CollectorMode mode() const override;
  [[nodiscard]] std::string device_uid() const override;
  [[nodiscard]] bool demand_active() const override;
  [[nodiscard]] bool demand_suppressed() const override;

  // Return true only when the command was delivered to the collector. The
  // request is rejected without dispatching when the observed state does not
  // permit it (start from stopped, stop from running).
  [[nodiscard]] bool request_start() override;
  [[nodiscard]] bool request_stop() override;
  [[nodiscard]] bool request_mode(collector_unit::CollectorMode mode) override;

  // Demand remains observable during an override. Call process() periodically
  // to release a pending automatic start after the configured delay and to
  // stop the collector once the run-on period after the last demand elapses.
  [[nodiscard]] bool update_demand(bool active);
  [[nodiscard]] bool process();

  // Publishes a CollectorFault Domain Event. State handling remains the
  // responsibility of the service that consumes the event.
  void report_fault(std::string reason);

private:
  template <typename CommandT> [[nodiscard]] bool dispatch_command() const;

  command_dispatcher::CommandDispatcher &dispatcher_;
  event_dispatcher::EventDispatcher &event_dispatcher_;
  const collector_unit::ICollectorUnit &unit_;
  event::DeviceUid device_uid_;
  event::EventIdGenerator &id_generator_;
  Clock clock_;
  std::chrono::milliseconds start_delay_;
  std::chrono::milliseconds run_on_;
  collector_unit::CollectorMode mode_ =
      collector_unit::CollectorMode::automatic;
  bool demand_active_ = false;
  std::optional<event::Timestamp> pending_demand_since_;
  // Set while AUTO output is running with no demand; cleared by new demand.
  std::optional<event::Timestamp> run_on_since_;

  [[nodiscard]] bool reconcile(bool automatic_start_immediate = false);
};

} // namespace open_extraction::collector_service
