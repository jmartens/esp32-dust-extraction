#pragma once

#include <optional>

namespace open_extraction::machine_demand_threshold {

struct MachineDemandThresholdProfile {
  float start_threshold_amps{};
  float stop_threshold_amps{};
};

enum class DemandTransition {
  none,
  started,
  stopped,
};

[[nodiscard]] bool
is_valid(const MachineDemandThresholdProfile &profile) noexcept;

class DemandThresholdEvaluator final {
public:
  // Create one evaluator per Machine Unit; missing or invalid profiles fail.
  [[nodiscard]] static std::optional<DemandThresholdEvaluator>
  create(const std::optional<MachineDemandThresholdProfile> &profile) noexcept;

  // The input is the calibrated primary RMS current in amperes.
  // Missing or invalid readings return nullopt without changing the state.
  [[nodiscard]] std::optional<DemandTransition>
  evaluate(std::optional<float> calibrated_rms_current_amps) noexcept;

  [[nodiscard]] bool demand_active() const noexcept { return demand_active_; }

private:
  explicit DemandThresholdEvaluator(
      MachineDemandThresholdProfile profile) noexcept;

  MachineDemandThresholdProfile profile_;
  bool demand_active_{false};
};

} // namespace open_extraction::machine_demand_threshold
