#pragma once

#include <optional>

#include "open_extraction/machine_demand_source/machine_demand_source.hpp"

namespace open_extraction::current_sensor {

class ICurrentSensor {
public:
  virtual ~ICurrentSensor() = default;

  // Returns calibrated primary RMS current, or nullopt when a reading fails.
  [[nodiscard]] virtual std::optional<float> read_current_rms() = 0;

  [[nodiscard]] virtual machine_demand_source::CalibrationState
  calibration() const = 0;

  // Calibration is supplied by the Controller-owned Unit profile.
  virtual void apply_calibration(
      const machine_demand_source::CalibrationState &calibration) = 0;
};

} // namespace open_extraction::current_sensor
