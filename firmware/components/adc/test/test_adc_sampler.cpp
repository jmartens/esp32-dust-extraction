#include <cstdint>

#include "adc/adc_sampler.hpp"
#include "unity.h"

namespace {

using open_extraction::adc::AdcConfig;
using open_extraction::adc::AdcSampler;

class FakeAdcSampler final : public AdcSampler {
public:
  FakeAdcSampler(const AdcConfig config, const int failing_read = -1)
      : config_(config), failing_read_(failing_read) {}

  [[nodiscard]] const AdcConfig &config() const override { return config_; }

  [[nodiscard]] std::optional<std::uint16_t> read_raw() override {
    const int index = reads_++;
    if (index == failing_read_) {
      return std::nullopt;
    }
    return static_cast<std::uint16_t>(100 + index);
  }

private:
  AdcConfig config_;
  int failing_read_;
  int reads_ = 0;
};

} // namespace

TEST_CASE("adc config validates sample rate", "[adc]") {
  TEST_ASSERT_TRUE(open_extraction::adc::is_valid({3, 1000}));
  TEST_ASSERT_FALSE(open_extraction::adc::is_valid({3, 0}));
  TEST_ASSERT_FALSE(open_extraction::adc::is_valid({3, 1'000'001}));
}

TEST_CASE("adc sample period follows sample rate", "[adc]") {
  TEST_ASSERT_EQUAL_UINT32(1000,
                           open_extraction::adc::sample_period_us({0, 1000}));
  TEST_ASSERT_EQUAL_UINT32(0, open_extraction::adc::sample_period_us({0, 0}));
}

TEST_CASE("adc sampler exposes configured channel and rate", "[adc]") {
  FakeAdcSampler sampler({6, 2000});

  TEST_ASSERT_EQUAL_UINT8(6, sampler.config().channel);
  TEST_ASSERT_EQUAL_UINT32(2000, sampler.config().sample_rate_hz);
}

TEST_CASE("adc sampler reads a raw sample", "[adc]") {
  FakeAdcSampler sampler({0, 1000});

  const auto sample = sampler.read_raw();

  TEST_ASSERT_TRUE(sample.has_value());
  TEST_ASSERT_EQUAL_UINT16(100, *sample);
}

TEST_CASE("adc sampler acquires a block of raw samples", "[adc]") {
  FakeAdcSampler sampler({0, 1000});
  std::uint16_t buffer[3] = {};

  TEST_ASSERT_EQUAL_size_t(3, sampler.read_raw(buffer, 3));
  TEST_ASSERT_EQUAL_UINT16(100, buffer[0]);
  TEST_ASSERT_EQUAL_UINT16(102, buffer[2]);
}

TEST_CASE("adc sampler block read stops at first failure", "[adc]") {
  FakeAdcSampler sampler({0, 1000}, 2);
  std::uint16_t buffer[4] = {};

  TEST_ASSERT_EQUAL_size_t(2, sampler.read_raw(buffer, 4));
  TEST_ASSERT_EQUAL_size_t(0, sampler.read_raw(nullptr, 4));
}
