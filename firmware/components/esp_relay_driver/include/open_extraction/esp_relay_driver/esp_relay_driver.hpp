#pragma once

#include "driver/gpio.h"
#include "relay/relay_driver.hpp"

namespace open_extraction::esp_relay_driver {

enum class RelayPolarity {
  active_high,
  active_low,
};

struct EspRelayConfig {
  gpio_num_t gpio_pin;
  RelayPolarity polarity;
};

class EspRelayDriver final : public relay::RelayDriver {
public:
  explicit EspRelayDriver(EspRelayConfig config);

  void on() override;
  void off() override;

  [[nodiscard]] relay::RelayState state() const override;

private:
  void set_state(relay::RelayState state);

  EspRelayConfig config_;
  relay::RelayState state_ = relay::RelayState::off;
};

} // namespace open_extraction::esp_relay_driver
