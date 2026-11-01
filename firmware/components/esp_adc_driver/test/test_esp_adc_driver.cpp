#include <cstdint>

#include "esp_timer.h"
#include "unity.h"

#include "open_extraction/esp_adc_driver/esp_adc_driver.hpp"

// On-target tests: require an ESP32 with an analog source on ADC1 channel 6
// (GPIO34 on ESP32). They are not part of the Linux host test build.
TEST_CASE("ESP ADC driver acquires raw samples at the configured rate",
          "[esp_adc_driver]") {
  open_extraction::esp_adc_driver::EspAdcDriver driver(
      {ADC_UNIT_1, ADC_CHANNEL_6, ADC_ATTEN_DB_12, 1000});

  TEST_ASSERT_EQUAL_UINT32(1000, driver.config().sample_rate_hz);
  TEST_ASSERT_TRUE(driver.read_raw().has_value());

  std::uint16_t buffer[100] = {};
  const std::int64_t start = esp_timer_get_time();
  TEST_ASSERT_EQUAL_size_t(100, driver.read_raw(buffer, 100));
  const std::int64_t elapsed_us = esp_timer_get_time() - start;

  TEST_ASSERT_INT64_WITHIN(20'000, 99'000, elapsed_us);
  for (const std::uint16_t sample : buffer) {
    TEST_ASSERT_LESS_OR_EQUAL_UINT16(4095, sample);
  }
}
