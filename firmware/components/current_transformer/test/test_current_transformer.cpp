#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "adc/adc_sampler.hpp"
#include "open_extraction/current_sensor/current_sensor.hpp"
#include "open_extraction/current_transformer/current_transformer.hpp"
#include "unity.h"

namespace {

using open_extraction::adc::AdcConfig;
using open_extraction::adc::AdcSampler;
using open_extraction::current_sensor::ICurrentSensor;
using open_extraction::current_transformer::CurrentTransformer;
using open_extraction::machine_demand_source::CalibrationState;

class FakeAdcSampler final : public AdcSampler {
public:
  FakeAdcSampler(const std::uint16_t *samples, const std::size_t sample_count,
                 const std::size_t failing_read = static_cast<std::size_t>(-1))
      : samples_(samples), sample_count_(sample_count),
        failing_read_(failing_read) {}

  [[nodiscard]] const AdcConfig &config() const override { return config_; }

  [[nodiscard]] std::optional<std::uint16_t> read_raw() override {
    if (reads_ == failing_read_ || reads_ >= sample_count_) {
      return std::nullopt;
    }
    return samples_[reads_++];
  }

  [[nodiscard]] std::size_t reads() const { return reads_; }

private:
  const std::uint16_t *samples_;
  std::size_t sample_count_;
  std::size_t failing_read_;
  std::size_t reads_{};
  AdcConfig config_{0, 1000};
};

class FakeCurrentSensor final : public ICurrentSensor {
public:
  [[nodiscard]] std::optional<float> read_current_rms() override {
    return 12.0F;
  }

  [[nodiscard]] CalibrationState calibration() const override {
    return calibration_;
  }

  void apply_calibration(const CalibrationState &calibration) override {
    calibration_ = calibration;
  }

private:
  CalibrationState calibration_{};
};

} // namespace

TEST_CASE("current transformer validates physical and ADC configuration",
          "[current_transformer]") {
  TEST_ASSERT_TRUE(
      open_extraction::current_transformer::is_valid({1000.0, 10.0, 0.01, 4}));
  TEST_ASSERT_FALSE(
      open_extraction::current_transformer::is_valid({0.0, 10.0, 0.01, 4}));
  TEST_ASSERT_FALSE(
      open_extraction::current_transformer::is_valid({1000.0, 0.0, 0.01, 4}));
  TEST_ASSERT_FALSE(
      open_extraction::current_transformer::is_valid({1000.0, 10.0, 0.0, 4}));
  TEST_ASSERT_FALSE(
      open_extraction::current_transformer::is_valid({1000.0, 10.0, 0.01, 0}));
}

TEST_CASE("current transformer converts offset ADC samples to primary RMS",
          "[current_transformer]") {
  constexpr std::array<std::uint16_t, 4> samples{1000, 1100, 900, 1000};
  FakeAdcSampler sampler(samples.data(), samples.size());
  CurrentTransformer transformer(sampler, {1000.0, 10.0, 0.01, samples.size()});

  const auto current = transformer.read_current_rms();

  TEST_ASSERT_TRUE(current.has_value());
  TEST_ASSERT_FLOAT_WITHIN(0.001F, 70.710678F, *current);
}

TEST_CASE("current transformer calculates sine RMS within five percent",
          "[current_transformer]") {
  constexpr std::size_t configured_window = 100;
  constexpr double sample_rate_hz = 1000.0;
  constexpr double frequency_hz = 50.0;
  constexpr double pi = 3.14159265358979323846;
  std::array<std::uint16_t, configured_window * 2> samples{};
  for (std::size_t index = 0; index < configured_window; ++index) {
    const double phase =
        2.0 * pi * frequency_hz * static_cast<double>(index) / sample_rate_hz;
    samples[index] =
        static_cast<std::uint16_t>(2048 + std::lround(100.0 * std::sin(phase)));
  }
  for (std::size_t index = configured_window; index < samples.size(); ++index) {
    samples[index] = 2548;
  }

  FakeAdcSampler sampler(samples.data(), samples.size());
  CurrentTransformer transformer(sampler,
                                 {100.0, 10.0, 0.01, configured_window});

  const auto current = transformer.read_current_rms();

  TEST_ASSERT_TRUE(current.has_value());
  constexpr float expected_current_rms = 7.071068F;
  TEST_ASSERT_FLOAT_WITHIN(expected_current_rms * 0.05F, expected_current_rms,
                           *current);
  TEST_ASSERT_EQUAL_size_t(configured_window, sampler.reads());
}

TEST_CASE("current transformer supports arbitrary ratios and burden values",
          "[current_transformer]") {
  constexpr std::array<std::uint16_t, 2> samples{900, 1100};
  FakeAdcSampler first_sampler(samples.data(), samples.size());
  FakeAdcSampler second_sampler(samples.data(), samples.size());
  FakeAdcSampler third_sampler(samples.data(), samples.size());
  CurrentTransformer first(first_sampler, {100.0, 10.0, 0.01, samples.size()});
  CurrentTransformer second(second_sampler,
                            {200.0, 10.0, 0.01, samples.size()});
  CurrentTransformer third(third_sampler, {100.0, 20.0, 0.01, samples.size()});

  const auto first_current = first.read_current_rms();
  const auto second_current = second.read_current_rms();
  const auto third_current = third.read_current_rms();

  TEST_ASSERT_TRUE(first_current.has_value());
  TEST_ASSERT_TRUE(second_current.has_value());
  TEST_ASSERT_TRUE(third_current.has_value());
  TEST_ASSERT_FLOAT_WITHIN(0.001F, 10.0F, *first_current);
  TEST_ASSERT_FLOAT_WITHIN(0.001F, 20.0F, *second_current);
  TEST_ASSERT_FLOAT_WITHIN(0.001F, 5.0F, *third_current);
}

TEST_CASE("current transformer applies Controller-supplied calibration",
          "[current_transformer]") {
  constexpr std::array<std::uint16_t, 2> samples{900, 1100};
  FakeAdcSampler sampler(samples.data(), samples.size());
  CurrentTransformer transformer(sampler, {100.0, 10.0, 0.01, samples.size()});
  ICurrentSensor &sensor = transformer;
  const CalibrationState supplied{true, 2.5F};

  sensor.apply_calibration(supplied);
  const auto current = sensor.read_current_rms();

  TEST_ASSERT_TRUE(sensor.calibration() == supplied);
  TEST_ASSERT_TRUE(current.has_value());
  TEST_ASSERT_FLOAT_WITHIN(0.001F, 25.0F, *current);
}

TEST_CASE("current transformer uses unity factor when uncalibrated",
          "[current_transformer]") {
  constexpr std::array<std::uint16_t, 2> samples{900, 1100};
  FakeAdcSampler sampler(samples.data(), samples.size());
  CurrentTransformer transformer(sampler, {100.0, 10.0, 0.01, samples.size()});
  transformer.apply_calibration({false, 3.0F});

  const auto current = transformer.read_current_rms();

  TEST_ASSERT_TRUE(current.has_value());
  TEST_ASSERT_FLOAT_WITHIN(0.001F, 10.0F, *current);
}

TEST_CASE("current transformer removes DC bias from a constant signal",
          "[current_transformer]") {
  constexpr std::array<std::uint16_t, 3> samples{1200, 1200, 1200};
  FakeAdcSampler sampler(samples.data(), samples.size());
  CurrentTransformer transformer(sampler, {100.0, 10.0, 0.01, samples.size()});

  const auto current = transformer.read_current_rms();

  TEST_ASSERT_TRUE(current.has_value());
  TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.0F, *current);
}

TEST_CASE("current transformer reports incomplete ADC reads",
          "[current_transformer]") {
  constexpr std::array<std::uint16_t, 4> samples{900, 1100, 900, 1100};
  FakeAdcSampler sampler(samples.data(), samples.size(), 2);
  CurrentTransformer transformer(sampler, {100.0, 10.0, 0.01, samples.size()});

  TEST_ASSERT_FALSE(transformer.read_current_rms().has_value());
}

TEST_CASE("current sensor interface supports another implementation",
          "[current_transformer]") {
  FakeCurrentSensor implementation;
  ICurrentSensor &sensor = implementation;
  const CalibrationState supplied{true, 1.5F};

  sensor.apply_calibration(supplied);
  const auto current = sensor.read_current_rms();

  TEST_ASSERT_TRUE(sensor.calibration() == supplied);
  TEST_ASSERT_TRUE(current.has_value());
  TEST_ASSERT_FLOAT_WITHIN(0.001F, 12.0F, *current);
}

TEST_CASE("current transformer reads windows larger than its sample buffer",
          "[current_transformer]") {
  std::array<std::uint16_t, 65> samples{};
  for (std::size_t index = 0; index < samples.size(); ++index) {
    samples[index] = index % 2 == 0 ? 900 : 1100;
  }
  FakeAdcSampler sampler(samples.data(), samples.size());
  CurrentTransformer transformer(sampler, {100.0, 10.0, 0.01, samples.size()});

  const auto current = transformer.read_current_rms();

  TEST_ASSERT_TRUE(current.has_value());
  TEST_ASSERT_FLOAT_WITHIN(0.01F, 10.0F, *current);
}

TEST_CASE("current transformer rejects invalid configuration and calibration",
          "[current_transformer]") {
  constexpr std::array<std::uint16_t, 2> samples{900, 1100};
  FakeAdcSampler sampler(samples.data(), samples.size());
  CurrentTransformer invalid_config(sampler, {0.0, 10.0, 0.01, samples.size()});
  CurrentTransformer invalid_calibration(sampler,
                                         {100.0, 10.0, 0.01, samples.size()});
  invalid_calibration.apply_calibration({true, 0.0F});

  TEST_ASSERT_FALSE(invalid_config.read_current_rms().has_value());
  TEST_ASSERT_FALSE(invalid_calibration.read_current_rms().has_value());
}
