#include "open_extraction/machine_demand_threshold/demand_threshold_evaluator.hpp"

#include <cmath>

namespace open_extraction::machine_demand_threshold {

bool is_valid(const MachineDemandThresholdProfile &profile) noexcept {
  return std::isfinite(profile.start_threshold_amps) &&
         std::isfinite(profile.stop_threshold_amps) &&
         profile.start_threshold_amps > 0.0F &&
         profile.stop_threshold_amps >= 0.0F &&
         profile.start_threshold_amps > profile.stop_threshold_amps;
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
    const std::optional<float> calibrated_rms_current_amps) noexcept {
  if (!calibrated_rms_current_amps ||
      !std::isfinite(*calibrated_rms_current_amps) ||
      *calibrated_rms_current_amps < 0.0F) {
    return std::nullopt;
  }

  if (!demand_active_ &&
      *calibrated_rms_current_amps >= profile_.start_threshold_amps) {
    demand_active_ = true;
    return DemandTransition::started;
  }

  if (demand_active_ &&
      *calibrated_rms_current_amps <= profile_.stop_threshold_amps) {
    demand_active_ = false;
    return DemandTransition::stopped;
  }

  return DemandTransition::none;
}

} // namespace open_extraction::machine_demand_threshold
