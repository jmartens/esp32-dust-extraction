#include "open_extraction/current_transformer/current_transformer.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace open_extraction::current_transformer {
namespace {

constexpr std::size_t sample_buffer_size = 64;

[[nodiscard]] bool
is_valid(const machine_demand_source::CalibrationState &calibration) noexcept {
  return std::isfinite(calibration.factor) && calibration.factor > 0.0F;
}

} // namespace

bool is_valid(const CurrentTransformerConfig &config) noexcept {
  return std::isfinite(config.turns_ratio) && config.turns_ratio > 0.0 &&
         std::isfinite(config.burden_resistance_ohms) &&
         config.burden_resistance_ohms > 0.0 &&
         std::isfinite(config.adc_volts_per_count) &&
         config.adc_volts_per_count > 0.0 && config.sample_count > 0;
}

CurrentTransformer::CurrentTransformer(adc::AdcSampler &sampler,
                                       const CurrentTransformerConfig config)
    : sampler_(sampler), config_(config) {}

std::optional<float> CurrentTransformer::read_current_rms() {
  if (!is_valid(config_) || !is_valid(calibration_)) {
    return std::nullopt;
  }

  std::array<std::uint16_t, sample_buffer_size> samples{};
  std::size_t samples_read = 0;
  double mean = 0.0;
  double sum_squared_deviations = 0.0;

  while (samples_read < config_.sample_count) {
    const std::size_t requested =
        std::min(config_.sample_count - samples_read, samples.size());
    const std::size_t received = sampler_.read_raw(samples.data(), requested);
    if (received != requested) {
      return std::nullopt;
    }

    for (std::size_t index = 0; index < received; ++index) {
      ++samples_read;
      const double value = static_cast<double>(samples[index]);
      const double delta = value - mean;
      mean += delta / static_cast<double>(samples_read);
      sum_squared_deviations += delta * (value - mean);
    }
  }

  const double rms_counts =
      std::sqrt(sum_squared_deviations / static_cast<double>(samples_read));
  const double calibration_factor =
      calibration_.calibrated ? calibration_.factor : 1.0;
  const double primary_current = rms_counts * config_.adc_volts_per_count /
                                 config_.burden_resistance_ohms *
                                 config_.turns_ratio * calibration_factor;
  if (!std::isfinite(primary_current)) {
    return std::nullopt;
  }

  const float current = static_cast<float>(primary_current);
  return std::isfinite(current) ? std::optional<float>(current) : std::nullopt;
}

machine_demand_source::CalibrationState
CurrentTransformer::calibration() const {
  return calibration_;
}

void CurrentTransformer::apply_calibration(
    const machine_demand_source::CalibrationState &calibration) {
  calibration_ = calibration;
}

} // namespace open_extraction::current_transformer
