# ESP ADC Driver

`EspAdcDriver` is the ESP-IDF (`adc_oneshot`) adapter for the portable
`adc::AdcSampler` interface. Consumers depend only on `adc::AdcSampler`, which
exposes the configured channel and sample rate and returns raw, unscaled
samples (`read_raw()` for one sample, `read_raw(buffer, count)` for a block
paced at the configured sample rate).

Configure the ADC unit, channel, attenuation and sample rate (1 Hz to 1 MHz)
through `EspAdcConfig`. Block reads are paced by busy-wait delays, so
achievable rates are limited by oneshot conversion time.

## Hardware testing

`test/test_esp_adc_driver.cpp` must run on a physical ESP32 with an analog
source on ADC1 channel 6 (GPIO34). It is not part of the Linux host build.
