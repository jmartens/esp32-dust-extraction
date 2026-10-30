#pragma once

#include <cstdint>

namespace open_extraction::machine_demand_source {

// Calibration supplied by the Controller-owned Unit configuration. A source
// only reports what it was given; it never owns or persists configuration.
struct CalibrationState {
  bool calibrated{false};
  float factor{1.0F};

  [[nodiscard]] friend constexpr bool
  operator==(const CalibrationState &, const CalibrationState &) = default;
};

struct MachineDemandDiagnostics {
  std::uint32_t demand_started_count{};
  std::uint32_t demand_stopped_count{};
  std::uint32_t read_error_count{};
};

class IMachineDemandSource {
public:
  virtual ~IMachineDemandSource() = default;

  [[nodiscard]] virtual bool demand_active() const = 0;
  [[nodiscard]] virtual MachineDemandDiagnostics diagnostics() const = 0;
  [[nodiscard]] virtual CalibrationState calibration() const = 0;

  // Applies Controller-provided calibration; the caller owns persistence.
  virtual void apply_calibration(const CalibrationState &calibration) = 0;
};

} // namespace open_extraction::machine_demand_source
