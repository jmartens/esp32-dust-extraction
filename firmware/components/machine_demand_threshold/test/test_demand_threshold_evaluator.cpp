#include <limits>
#include <optional>

#include "open_extraction/machine_demand_threshold/demand_threshold_evaluator.hpp"
#include "unity.h"

namespace {

using open_extraction::machine_demand_threshold::DemandThresholdEvaluator;
using open_extraction::machine_demand_threshold::DemandTransition;
using open_extraction::machine_demand_threshold::MachineDemandThresholdProfile;
using open_extraction::machine_demand_threshold::MonotonicClock;

} // namespace

TEST_CASE("demand threshold profile requires finite ordered thresholds",
          "[machine_demand_threshold]") {
  TEST_ASSERT_TRUE(open_extraction::machine_demand_threshold::is_valid(
      {8.0F, 2.0F, std::chrono::milliseconds(0)}));
  TEST_ASSERT_TRUE(open_extraction::machine_demand_threshold::is_valid(
      {8.0F, 0.0F, std::chrono::milliseconds(250)}));
  TEST_ASSERT_FALSE(open_extraction::machine_demand_threshold::is_valid(
      {0.0F, 0.0F, std::chrono::milliseconds(0)}));
  TEST_ASSERT_FALSE(open_extraction::machine_demand_threshold::is_valid(
      {2.0F, 2.0F, std::chrono::milliseconds(0)}));
  TEST_ASSERT_FALSE(open_extraction::machine_demand_threshold::is_valid(
      {2.0F, 3.0F, std::chrono::milliseconds(0)}));
  TEST_ASSERT_FALSE(open_extraction::machine_demand_threshold::is_valid(
      {std::numeric_limits<float>::infinity(), 2.0F,
       std::chrono::milliseconds(0)}));
  TEST_ASSERT_FALSE(open_extraction::machine_demand_threshold::is_valid(
      {8.0F, std::numeric_limits<float>::quiet_NaN(),
       std::chrono::milliseconds(0)}));
  TEST_ASSERT_FALSE(open_extraction::machine_demand_threshold::is_valid(
      {8.0F, -1.0F, std::chrono::milliseconds(0)}));
  TEST_ASSERT_FALSE(open_extraction::machine_demand_threshold::is_valid(
      {8.0F, 2.0F, std::chrono::milliseconds(-1)}));
  TEST_ASSERT_FALSE(open_extraction::machine_demand_threshold::is_valid(
      {8.0F, 2.0F, std::nullopt}));
}

TEST_CASE("demand threshold evaluator rejects missing and invalid profiles",
          "[machine_demand_threshold]") {
  const std::optional<MachineDemandThresholdProfile> missing_profile;

  TEST_ASSERT_FALSE(
      DemandThresholdEvaluator::create(missing_profile).has_value());
  TEST_ASSERT_FALSE(DemandThresholdEvaluator::create(
                        MachineDemandThresholdProfile{
                            2.0F, 3.0F, std::chrono::milliseconds(0)})
                        .has_value());
  TEST_ASSERT_FALSE(DemandThresholdEvaluator::create(
                        MachineDemandThresholdProfile{
                            8.0F, 2.0F, std::chrono::milliseconds(-1)})
                        .has_value());
  TEST_ASSERT_FALSE(DemandThresholdEvaluator::create(
                        MachineDemandThresholdProfile{8.0F, 2.0F, std::nullopt})
                        .has_value());
}

TEST_CASE("demand threshold evaluator debounces both demand transitions",
          "[machine_demand_threshold]") {
  auto evaluator =
      DemandThresholdEvaluator::create(MachineDemandThresholdProfile{
          8.0F, 2.0F, std::chrono::milliseconds(100)});
  const auto start = MonotonicClock::time_point{};

  TEST_ASSERT_TRUE(evaluator.has_value());
  TEST_ASSERT_FALSE(evaluator->demand_active());
  TEST_ASSERT_TRUE(evaluator->evaluate(7.99F, start) == DemandTransition::none);
  TEST_ASSERT_TRUE(evaluator->evaluate(8.0F, start) == DemandTransition::none);
  TEST_ASSERT_FALSE(evaluator->demand_active());
  TEST_ASSERT_TRUE(
      evaluator->evaluate(8.0F, start + std::chrono::milliseconds(99)) ==
      DemandTransition::none);
  TEST_ASSERT_FALSE(evaluator->demand_active());
  TEST_ASSERT_TRUE(
      evaluator->evaluate(8.0F, start + std::chrono::milliseconds(100)) ==
      DemandTransition::started);
  TEST_ASSERT_TRUE(evaluator->demand_active());
  TEST_ASSERT_TRUE(
      evaluator->evaluate(2.0F, start + std::chrono::milliseconds(101)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(evaluator->demand_active());
  TEST_ASSERT_TRUE(
      evaluator->evaluate(2.0F, start + std::chrono::milliseconds(201)) ==
      DemandTransition::stopped);
  TEST_ASSERT_FALSE(evaluator->demand_active());
}

TEST_CASE("demand threshold evaluator cancels candidates that are not stable",
          "[machine_demand_threshold]") {
  auto evaluator =
      DemandThresholdEvaluator::create(MachineDemandThresholdProfile{
          8.0F, 2.0F, std::chrono::milliseconds(100)});
  const auto start = MonotonicClock::time_point{};
  const std::optional<float> missing_reading;

  TEST_ASSERT_TRUE(evaluator.has_value());
  TEST_ASSERT_TRUE(evaluator->evaluate(8.0F, start) == DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(7.0F, start + std::chrono::milliseconds(50)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(8.0F, start + std::chrono::milliseconds(60)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(8.0F, start + std::chrono::milliseconds(159)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(8.0F, start + std::chrono::milliseconds(160)) ==
      DemandTransition::started);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(2.0F, start + std::chrono::milliseconds(170)) ==
      DemandTransition::none);
  TEST_ASSERT_FALSE(
      evaluator
          ->evaluate(missing_reading, start + std::chrono::milliseconds(180))
          .has_value());
  TEST_ASSERT_FALSE(
      evaluator->evaluate(-1.0F, start + std::chrono::milliseconds(190))
          .has_value());
  TEST_ASSERT_FALSE(evaluator
                        ->evaluate(std::numeric_limits<float>::infinity(),
                                   start + std::chrono::milliseconds(200))
                        .has_value());
  TEST_ASSERT_TRUE(
      evaluator->evaluate(2.0F, start + std::chrono::milliseconds(201)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(2.0F, start + std::chrono::milliseconds(300)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(2.0F, start + std::chrono::milliseconds(301)) ==
      DemandTransition::stopped);
  TEST_ASSERT_FALSE(evaluator->demand_active());
}

TEST_CASE(
    "zero debounce disables delay and unchanged readings emit no transitions",
    "[machine_demand_threshold]") {
  auto evaluator = DemandThresholdEvaluator::create(
      MachineDemandThresholdProfile{8.0F, 2.0F, std::chrono::milliseconds(0)});
  const auto start = MonotonicClock::time_point{};

  TEST_ASSERT_TRUE(evaluator.has_value());
  TEST_ASSERT_TRUE(evaluator->evaluate(8.0F, start) ==
                   DemandTransition::started);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(9.0F, start + std::chrono::milliseconds(1)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(5.0F, start + std::chrono::milliseconds(2)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(2.0F, start + std::chrono::milliseconds(3)) ==
      DemandTransition::stopped);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(1.0F, start + std::chrono::milliseconds(4)) ==
      DemandTransition::none);
}

TEST_CASE("demand debounce durations are independent per unit",
          "[machine_demand_threshold]") {
  auto first_unit = DemandThresholdEvaluator::create(
      MachineDemandThresholdProfile{8.0F, 2.0F, std::chrono::milliseconds(50)});
  auto second_unit =
      DemandThresholdEvaluator::create(MachineDemandThresholdProfile{
          8.0F, 2.0F, std::chrono::milliseconds(100)});
  const auto start = MonotonicClock::time_point{};

  TEST_ASSERT_TRUE(first_unit.has_value());
  TEST_ASSERT_TRUE(second_unit.has_value());
  TEST_ASSERT_TRUE(first_unit->evaluate(8.0F, start) == DemandTransition::none);
  TEST_ASSERT_TRUE(second_unit->evaluate(8.0F, start) ==
                   DemandTransition::none);
  TEST_ASSERT_TRUE(
      first_unit->evaluate(8.0F, start + std::chrono::milliseconds(50)) ==
      DemandTransition::started);
  TEST_ASSERT_TRUE(
      second_unit->evaluate(8.0F, start + std::chrono::milliseconds(50)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(first_unit->demand_active());
  TEST_ASSERT_FALSE(second_unit->demand_active());
}

TEST_CASE(
    "demand threshold evaluator resets pending debounce on clock regression",
    "[machine_demand_threshold]") {
  auto evaluator =
      DemandThresholdEvaluator::create(MachineDemandThresholdProfile{
          8.0F, 2.0F, std::chrono::milliseconds(100)});
  const auto start = MonotonicClock::time_point{};

  TEST_ASSERT_TRUE(evaluator.has_value());
  TEST_ASSERT_TRUE(
      evaluator->evaluate(8.0F, start + std::chrono::milliseconds(50)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(8.0F, start + std::chrono::milliseconds(40)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(8.0F, start + std::chrono::milliseconds(139)) ==
      DemandTransition::none);
  TEST_ASSERT_TRUE(
      evaluator->evaluate(8.0F, start + std::chrono::milliseconds(140)) ==
      DemandTransition::started);
}
