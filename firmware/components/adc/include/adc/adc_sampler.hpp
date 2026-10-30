#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace open_extraction::adc {

constexpr std::uint32_t max_sample_rate_hz = 1'000'000;

struct AdcConfig {
  std::uint8_t channel;
  std::uint32_t sample_rate_hz;
};

[[nodiscard]] constexpr bool is_valid(const AdcConfig &config) {
  return config.sample_rate_hz > 0 &&
         config.sample_rate_hz <= max_sample_rate_hz;
}

[[nodiscard]] constexpr std::uint32_t
sample_period_us(const AdcConfig &config) {
  return is_valid(config) ? 1'000'000U / config.sample_rate_hz : 0U;
}

// Hardware independent source of raw, unscaled ADC conversion results.
class AdcSampler {
public:
  virtual ~AdcSampler() = default;

  [[nodiscard]] virtual const AdcConfig &config() const = 0;

  // Returns one raw conversion, or nullopt if the conversion failed.
  [[nodiscard]] virtual std::optional<std::uint16_t> read_raw() = 0;

  // Acquires up to `count` samples at the configured rate. Returns the number
  // of samples stored; stops early on the first failed conversion.
  [[nodiscard]] virtual std::size_t read_raw(std::uint16_t *buffer,
                                             std::size_t count) {
    std::size_t stored = 0;
    while (buffer != nullptr && stored < count) {
      const auto sample = read_raw();
      if (!sample) {
        break;
      }
      buffer[stored++] = *sample;
    }
    return stored;
  }
};

} // namespace open_extraction::adc
