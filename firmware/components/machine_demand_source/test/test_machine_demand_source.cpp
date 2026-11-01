#include "open_extraction/machine_demand_source/machine_demand_source.hpp"
#include "unity.h"

namespace {

using open_extraction::machine_demand_source::CalibrationState;
using open_extraction::machine_demand_source::IMachineDemandSource;
using open_extraction::machine_demand_source::MachineDemandDiagnostics;

class FakeDemandSource final : public IMachineDemandSource {
public:
  [[nodiscard]] bool demand_active() const override { return active_; }
  [[nodiscard]] MachineDemandDiagnostics diagnostics() const override {
    return diagnostics_;
  }
  [[nodiscard]] CalibrationState calibration() const override {
    return calibration_;
  }
  void apply_calibration(const CalibrationState &calibration) override {
    calibration_ = calibration;
  }

  void set_active(bool active) {
    if (active && !active_) {
      ++diagnostics_.demand_started_count;
    } else if (!active && active_) {
      ++diagnostics_.demand_stopped_count;
    }
    active_ = active;
  }

private:
  bool active_{false};
  MachineDemandDiagnostics diagnostics_{};
  CalibrationState calibration_{};
};

} // namespace

TEST_CASE("demand source is inactive and uncalibrated by default",
          "[machine_demand_source]") {
  const FakeDemandSource fake;
  const IMachineDemandSource &source = fake;

  TEST_ASSERT_FALSE(source.demand_active());
  TEST_ASSERT_FALSE(source.calibration().calibrated);
  TEST_ASSERT_EQUAL_UINT32(0, source.diagnostics().demand_started_count);
  TEST_ASSERT_EQUAL_UINT32(0, source.diagnostics().demand_stopped_count);
  TEST_ASSERT_EQUAL_UINT32(0, source.diagnostics().read_error_count);
}

TEST_CASE("demand source exposes demand state and diagnostics",
          "[machine_demand_source]") {
  FakeDemandSource fake;
  const IMachineDemandSource &source = fake;

  fake.set_active(true);
  TEST_ASSERT_TRUE(source.demand_active());
  TEST_ASSERT_EQUAL_UINT32(1, source.diagnostics().demand_started_count);

  fake.set_active(false);
  TEST_ASSERT_FALSE(source.demand_active());
  TEST_ASSERT_EQUAL_UINT32(1, source.diagnostics().demand_stopped_count);
}

TEST_CASE("demand source reports Controller-supplied calibration",
          "[machine_demand_source]") {
  FakeDemandSource fake;
  IMachineDemandSource &source = fake;
  const CalibrationState supplied{true, 2.5F};

  source.apply_calibration(supplied);

  TEST_ASSERT_TRUE(source.calibration() == supplied);
}
