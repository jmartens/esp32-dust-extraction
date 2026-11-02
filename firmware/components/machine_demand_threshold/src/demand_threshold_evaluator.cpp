#include "open_extraction/machine_demand_threshold/demand_threshold_evaluator.hpp"

#include <cmath>

namespace open_extraction::machine_demand_threshold {

bool is_valid(const MachineDemandThresholdProfile &profile) noexcept {
  return std::isfinite(profile.start_threshold_amps) &&
         std::isfinite(profile.stop_threshold_amps) &&
         profile.start_threshold_amps > 0.0F &&
         profile.stop_threshold_amps >= 0.0F &&
         profile.start_threshold_amps > profile.stop_threshold_amps &&
         profile.debounce_duration &&
         *profile.debounce_duration >= std::chrono::milliseconds::zero();
}

std::optional<DemandThresholdEvaluator> DemandThresholdEvaluator::create(
    const std::optional<MachineDemandThresholdProfile> &profile) noexcept {
  if (!profile || !is_valid(*profile)) {
    return std::nullopt;
  }
  return DemandThresholdEvaluator(*profile);
}

DemandThresholdEvaluator::DemandThresholdEvaluator(
    const MachineDemandThresholdProfile profile) noexcept
    : profile_(profile) {}

std::optional<DemandTransition> DemandThresholdEvaluator::evaluate(
    const std::optional<float> calibrated_rms_current_amps,
    const MonotonicClock::time_point now) noexcept {
  if (!calibrated_rms_current_amps ||
      !std::isfinite(*calibrated_rms_current_amps) ||
      *calibrated_rms_current_amps < 0.0F) {
    pending_demand_active_.reset();
    pending_since_.reset();
    return std::nullopt;
  }

  const bool requested_demand_active =
      demand_active_
          ? *calibrated_rms_current_amps > profile_.stop_threshold_amps
          : *calibrated_rms_current_amps >= profile_.start_threshold_amps;
  if (requested_demand_active == demand_active_) {
    pending_demand_active_.reset();
    pending_since_.reset();
    return DemandTransition::none;
  }

  if (!pending_demand_active_ ||
      *pending_demand_active_ != requested_demand_active || !pending_since_) {
    pending_demand_active_ = requested_demand_active;
    pending_since_ = now;
  } else if (now < *pending_since_) {
    pending_since_ = now;
  }

  if (now - *pending_since_ < *profile_.debounce_duration) {
    return DemandTransition::none;
  }

  demand_active_ = requested_demand_active;
  pending_demand_active_.reset();
  pending_since_.reset();
  return demand_active_ ? DemandTransition::started : DemandTransition::stopped;
}

} // namespace open_extraction::machine_demand_threshold
