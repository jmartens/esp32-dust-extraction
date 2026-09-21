#include "open_extraction/esp_relay_driver/esp_relay_driver.hpp"

#include <cstdint>

#include "esp_check.h"

namespace open_extraction::esp_relay_driver {

EspRelayDriver::EspRelayDriver(const EspRelayConfig config) : config_(config) {
  const gpio_config_t gpio_configuration{
      .pin_bit_mask = 1ULL << static_cast<std::uint32_t>(config_.gpio_pin),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };

  ESP_ERROR_CHECK(gpio_config(&gpio_configuration));
  off();
}

void EspRelayDriver::on() { set_state(relay::RelayState::on); }

void EspRelayDriver::off() { set_state(relay::RelayState::off); }

relay::RelayState EspRelayDriver::state() const { return state_; }

void EspRelayDriver::set_state(const relay::RelayState state) {
  const bool is_active = state == relay::RelayState::on;
  const bool output_high =
      config_.polarity == RelayPolarity::active_high ? is_active : !is_active;

  ESP_ERROR_CHECK(gpio_set_level(config_.gpio_pin, output_high ? 1 : 0));
  state_ = state;
}

} // namespace open_extraction::esp_relay_driver
