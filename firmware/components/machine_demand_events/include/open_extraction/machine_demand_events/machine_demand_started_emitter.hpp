#pragma once

#include <functional>
#include <optional>
#include <string>

#include "open_extraction/event/event.hpp"
#include "open_extraction/event_dispatcher/event_dispatcher.hpp"
#include "open_extraction/machine_demand_threshold/demand_threshold_evaluator.hpp"

namespace open_extraction::machine_demand_events {

class MachineDemandStartedEmitter final {
public:
  using Clock = std::function<event::Timestamp()>;

  MachineDemandStartedEmitter(
      machine_demand_threshold::DemandThresholdEvaluator evaluator,
      event::DeviceUid device_uid, std::string unit_id,
      event::EventIdGenerator &id_generator, Clock clock,
      event_dispatcher::EventDispatcher &dispatcher);

  [[nodiscard]] std::optional<machine_demand_threshold::DemandTransition>
  evaluate(std::optional<float> calibrated_rms_current_amps,
           machine_demand_threshold::MonotonicClock::time_point now);

  [[nodiscard]] bool demand_active() const noexcept;

private:
  machine_demand_threshold::DemandThresholdEvaluator evaluator_;
  event::DeviceUid device_uid_;
  std::string unit_id_;
  event::EventIdGenerator &id_generator_;
  Clock clock_;
  event_dispatcher::EventDispatcher &dispatcher_;
};

} // namespace open_extraction::machine_demand_events
