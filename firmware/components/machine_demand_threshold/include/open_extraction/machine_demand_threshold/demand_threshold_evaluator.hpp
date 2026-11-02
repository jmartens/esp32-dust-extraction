#pragma once

#include <chrono>
#include <optional>

namespace open_extraction::machine_demand_threshold {

struct MachineDemandThresholdProfile {
  float start_threshold_amps{};
  float stop_threshold_amps{};
  std::optional<std::chrono::milliseconds> debounce_duration;
};

enum class DemandTransition {
  none,
  started,
  stopped,
};

using MonotonicClock = std::chrono::steady_clock;

[[nodiscard]] bool
is_valid(const MachineDemandThresholdProfile &profile) noexcept;

class DemandThresholdEvaluator final {
public:
  // Create one evaluator per Machine Unit; missing or invalid profiles fail.
  [[nodiscard]] static std::optional<DemandThresholdEvaluator>
  create(const std::optional<MachineDemandThresholdProfile> &profile) noexcept;

  // The input is the calibrated primary RMS current in amperes. now must be
  // from a monotonic clock. Missing or invalid readings cancel pending changes.
  [[nodiscard]] std::optional<DemandTransition>
  evaluate(std::optional<float> calibrated_rms_current_amps,
           MonotonicClock::time_point now) noexcept;

  [[nodiscard]] bool demand_active() const noexcept { return demand_active_; }

private:
  explicit DemandThresholdEvaluator(
      MachineDemandThresholdProfile profile) noexcept;

  MachineDemandThresholdProfile profile_;
  bool demand_active_{false};
  std::optional<bool> pending_demand_active_;
  std::optional<MonotonicClock::time_point> pending_since_;
};

} // namespace open_extraction::machine_demand_threshold
