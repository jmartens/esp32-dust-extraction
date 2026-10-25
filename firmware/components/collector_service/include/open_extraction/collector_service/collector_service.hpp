#pragma once

#include <functional>
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
                   event::EventIdGenerator &id_generator, Clock clock);

  [[nodiscard]] collector_unit::CollectorState state() const override;
  [[nodiscard]] std::string device_uid() const override;

  // Return true only when the command was delivered to the collector. The
  // request is rejected without dispatching when the observed state does not
  // permit it (start from stopped, stop from running).
  [[nodiscard]] bool request_start() override;
  [[nodiscard]] bool request_stop() override;

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
};

} // namespace open_extraction::collector_service
