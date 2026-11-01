#include "open_extraction/esp_adc_driver/esp_adc_driver.hpp"

#include "esp_check.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

namespace open_extraction::esp_adc_driver {

EspAdcDriver::EspAdcDriver(const EspAdcConfig config)
    : esp_config_(config), config_{static_cast<std::uint8_t>(config.channel),
                                   config.sample_rate_hz} {
  ESP_ERROR_CHECK(adc::is_valid(config_) ? ESP_OK : ESP_ERR_INVALID_ARG);

  const adc_oneshot_unit_init_cfg_t unit_config{
      .unit_id = esp_config_.unit,
      .clk_src = static_cast<adc_oneshot_clk_src_t>(0),
      .ulp_mode = ADC_ULP_MODE_DISABLE,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config, &handle_));

  const adc_oneshot_chan_cfg_t channel_config{
      .atten = esp_config_.attenuation,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(handle_, esp_config_.channel,
                                             &channel_config));
}

EspAdcDriver::~EspAdcDriver() {
  ESP_ERROR_CHECK(adc_oneshot_del_unit(handle_));
}

const adc::AdcConfig &EspAdcDriver::config() const { return config_; }

std::optional<std::uint16_t> EspAdcDriver::read_raw() {
  int raw = 0;
  if (adc_oneshot_read(handle_, esp_config_.channel, &raw) != ESP_OK) {
    return std::nullopt;
  }
  return static_cast<std::uint16_t>(raw);
}

std::size_t EspAdcDriver::read_raw(std::uint16_t *buffer,
                                   const std::size_t count) {
  const std::int64_t period_us = adc::sample_period_us(config_);
  std::int64_t next_us = esp_timer_get_time();
  std::size_t stored = 0;

  while (buffer != nullptr && stored < count) {
    const auto sample = read_raw();
    if (!sample) {
      break;
    }
    buffer[stored++] = *sample;

    next_us += period_us;
    const std::int64_t wait_us = next_us - esp_timer_get_time();
    if (stored < count && wait_us > 0) {
      esp_rom_delay_us(static_cast<std::uint32_t>(wait_us));
    }
  }
  return stored;
}

} // namespace open_extraction::esp_adc_driver
