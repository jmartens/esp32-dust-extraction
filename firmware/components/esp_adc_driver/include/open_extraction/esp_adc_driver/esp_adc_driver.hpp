#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "adc/adc_sampler.hpp"
#include "esp_adc/adc_oneshot.h"

namespace open_extraction::esp_adc_driver {

struct EspAdcConfig {
  adc_unit_t unit;
  adc_channel_t channel;
  adc_atten_t attenuation;
  std::uint32_t sample_rate_hz;
};

class EspAdcDriver final : public adc::AdcSampler {
public:
  // Aborts if the configuration is invalid or the ADC cannot be initialised.
  explicit EspAdcDriver(EspAdcConfig config);
  ~EspAdcDriver() override;

  EspAdcDriver(const EspAdcDriver &) = delete;
  EspAdcDriver &operator=(const EspAdcDriver &) = delete;

  [[nodiscard]] const adc::AdcConfig &config() const override;
  [[nodiscard]] std::optional<std::uint16_t> read_raw() override;
  [[nodiscard]] std::size_t read_raw(std::uint16_t *buffer,
                                     std::size_t count) override;

private:
  EspAdcConfig esp_config_;
  adc::AdcConfig config_;
  adc_oneshot_unit_handle_t handle_ = nullptr;
};

} // namespace open_extraction::esp_adc_driver
