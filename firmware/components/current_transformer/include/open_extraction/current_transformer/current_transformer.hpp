#pragma once

#include <cstddef>
#include <optional>

#include "adc/adc_sampler.hpp"
#include "open_extraction/current_sensor/current_sensor.hpp"

namespace open_extraction::current_transformer {

struct CurrentTransformerConfig {
  double turns_ratio{};
  double burden_resistance_ohms{};
  double adc_volts_per_count{};
  std::size_t sample_count{};
};

[[nodiscard]] bool is_valid(const CurrentTransformerConfig &config) noexcept;

class CurrentTransformer final : public current_sensor::ICurrentSensor {
public:
  CurrentTransformer(adc::AdcSampler &sampler, CurrentTransformerConfig config);

  [[nodiscard]] std::optional<float> read_current_rms() override;
  [[nodiscard]] machine_demand_source::CalibrationState
  calibration() const override;
  void apply_calibration(
      const machine_demand_source::CalibrationState &calibration) override;

private:
  adc::AdcSampler &sampler_;
  CurrentTransformerConfig config_;
  machine_demand_source::CalibrationState calibration_{};
};

} // namespace open_extraction::current_transformer
