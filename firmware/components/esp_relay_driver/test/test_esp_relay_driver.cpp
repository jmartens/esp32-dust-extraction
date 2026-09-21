#include "driver/gpio.h"
#include "unity.h"

#include "open_extraction/esp_relay_driver/esp_relay_driver.hpp"

namespace {

using open_extraction::esp_relay_driver::EspRelayConfig;
using open_extraction::esp_relay_driver::EspRelayDriver;
using open_extraction::esp_relay_driver::RelayPolarity;

} // namespace

TEST_CASE("ESP relay driver controls an active-high relay",
          "[esp_relay_driver]") {
  constexpr gpio_num_t gpio_pin = GPIO_NUM_2;
  EspRelayDriver relay({gpio_pin, RelayPolarity::active_high});

  TEST_ASSERT_TRUE(relay.is_off());
  TEST_ASSERT_EQUAL(GPIO_LEVEL_LOW, gpio_get_level(gpio_pin));

  relay.on();

  TEST_ASSERT_TRUE(relay.is_on());
  TEST_ASSERT_EQUAL(GPIO_LEVEL_HIGH, gpio_get_level(gpio_pin));

  relay.off();

  TEST_ASSERT_TRUE(relay.is_off());
  TEST_ASSERT_EQUAL(GPIO_LEVEL_LOW, gpio_get_level(gpio_pin));
}

TEST_CASE("ESP relay driver controls an active-low relay",
          "[esp_relay_driver]") {
  constexpr gpio_num_t gpio_pin = GPIO_NUM_4;
  EspRelayDriver relay({gpio_pin, RelayPolarity::active_low});

  TEST_ASSERT_TRUE(relay.is_off());
  TEST_ASSERT_EQUAL(GPIO_LEVEL_HIGH, gpio_get_level(gpio_pin));

  relay.on();

  TEST_ASSERT_TRUE(relay.is_on());
  TEST_ASSERT_EQUAL(GPIO_LEVEL_LOW, gpio_get_level(gpio_pin));

  relay.off();

  TEST_ASSERT_TRUE(relay.is_off());
  TEST_ASSERT_EQUAL(GPIO_LEVEL_HIGH, gpio_get_level(gpio_pin));
}
