#include "open_extraction/machine_demand_events/machine_demand_started_emitter.hpp"

#include <utility>

#include "open_extraction/machine_demand_events/machine_demand_started.hpp"

namespace open_extraction::machine_demand_events {

MachineDemandStartedEmitter::MachineDemandStartedEmitter(
    machine_demand_threshold::DemandThresholdEvaluator evaluator,
    event::DeviceUid device_uid, std::string unit_id,
    event::EventIdGenerator &id_generator, Clock clock,
    event_dispatcher::EventDispatcher &dispatcher)
    : evaluator_(std::move(evaluator)), device_uid_(std::move(device_uid)),
      unit_id_(std::move(unit_id)), id_generator_(id_generator),
      clock_(std::move(clock)), dispatcher_(dispatcher) {}

std::optional<machine_demand_threshold::DemandTransition>
MachineDemandStartedEmitter::evaluate(
    std::optional<float> calibrated_rms_current_amps,
    machine_demand_threshold::MonotonicClock::time_point now) {
  const auto transition = evaluator_.evaluate(calibrated_rms_current_amps, now);
  if (transition == machine_demand_threshold::DemandTransition::started) {
    const MachineDemandStarted started(id_generator_.generate(), clock_(),
                                       device_uid_, unit_id_);
    dispatcher_.publish(started);
  }
  return transition;
}

bool MachineDemandStartedEmitter::demand_active() const noexcept {
  return evaluator_.demand_active();
}

} // namespace open_extraction::machine_demand_events
