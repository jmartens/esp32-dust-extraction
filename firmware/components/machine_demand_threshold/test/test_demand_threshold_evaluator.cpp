#include <limits>
#include <optional>

#include "open_extraction/machine_demand_threshold/demand_threshold_evaluator.hpp"
#include "unity.h"

namespace {

using open_extraction::machine_demand_threshold::DemandThresholdEvaluator;
using open_extraction::machine_demand_threshold::DemandTransition;
using open_extraction::machine_demand_threshold::MachineDemandThresholdProfile;

} // namespace

TEST_CASE("demand threshold profile requires finite ordered thresholds",
          "[machine_demand_threshold]") {
  TEST_ASSERT_TRUE(
      open_extraction::machine_demand_threshold::is_valid({8.0F, 2.0F}));
  TEST_ASSERT_TRUE(
      open_extraction::machine_demand_threshold::is_valid({8.0F, 0.0F}));
  TEST_ASSERT_FALSE(
      open_extraction::machine_demand_threshold::is_valid({0.0F, 0.0F}));
  TEST_ASSERT_FALSE(
      open_extraction::machine_demand_threshold::is_valid({2.0F, 2.0F}));
  TEST_ASSERT_FALSE(
      open_extraction::machine_demand_threshold::is_valid({2.0F, 3.0F}));
  TEST_ASSERT_FALSE(open_extraction::machine_demand_threshold::is_valid(
      {std::numeric_limits<float>::infinity(), 2.0F}));
  TEST_ASSERT_FALSE(open_extraction::machine_demand_threshold::is_valid(
      {8.0F, std::numeric_limits<float>::quiet_NaN()}));
  TEST_ASSERT_FALSE(
      open_extraction::machine_demand_threshold::is_valid({8.0F, -1.0F}));
}

TEST_CASE("demand threshold evaluator rejects missing and invalid profiles",
          "[machine_demand_threshold]") {
  const std::optional<MachineDemandThresholdProfile> missing_profile;

  TEST_ASSERT_FALSE(
      DemandThresholdEvaluator::create(missing_profile).has_value());
  TEST_ASSERT_FALSE(DemandThresholdEvaluator::create(
                        MachineDemandThresholdProfile{2.0F, 3.0F})
                        .has_value());
}

TEST_CASE("demand threshold evaluator starts at the configured start threshold",
          "[machine_demand_threshold]") {
  auto evaluator = DemandThresholdEvaluator::create(
      MachineDemandThresholdProfile{8.0F, 2.0F});

  TEST_ASSERT_TRUE(evaluator.has_value());
  TEST_ASSERT_FALSE(evaluator->demand_active());
  TEST_ASSERT_TRUE(evaluator->evaluate(7.99F) == DemandTransition::none);
  TEST_ASSERT_TRUE(evaluator->evaluate(8.0F) == DemandTransition::started);
  TEST_ASSERT_TRUE(evaluator->demand_active());
}

TEST_CASE("demand threshold evaluator maintains demand within hysteresis band",
          "[machine_demand_threshold]") {
  auto evaluator = DemandThresholdEvaluator::create(
      MachineDemandThresholdProfile{8.0F, 2.0F});

  TEST_ASSERT_TRUE(evaluator.has_value());
  TEST_ASSERT_TRUE(evaluator->evaluate(9.0F) == DemandTransition::started);
  TEST_ASSERT_TRUE(evaluator->evaluate(5.0F) == DemandTransition::none);
  TEST_ASSERT_TRUE(evaluator->demand_active());
  TEST_ASSERT_TRUE(evaluator->evaluate(2.0F) == DemandTransition::stopped);
  TEST_ASSERT_FALSE(evaluator->demand_active());
}

TEST_CASE("demand threshold evaluator ignores missing or invalid readings",
          "[machine_demand_threshold]") {
  auto evaluator = DemandThresholdEvaluator::create(
      MachineDemandThresholdProfile{8.0F, 2.0F});
  const std::optional<float> missing_reading;

  TEST_ASSERT_TRUE(evaluator.has_value());
  TEST_ASSERT_FALSE(evaluator->evaluate(missing_reading).has_value());
  TEST_ASSERT_FALSE(evaluator->evaluate(-1.0F).has_value());
  TEST_ASSERT_FALSE(
      evaluator->evaluate(std::numeric_limits<float>::infinity()).has_value());
  TEST_ASSERT_FALSE(evaluator->demand_active());
  TEST_ASSERT_TRUE(evaluator->evaluate(8.0F) == DemandTransition::started);
  TEST_ASSERT_FALSE(evaluator->evaluate(missing_reading).has_value());
  TEST_ASSERT_TRUE(evaluator->demand_active());
}

TEST_CASE("demand thresholds are independently configured per unit",
          "[machine_demand_threshold]") {
  auto first_unit = DemandThresholdEvaluator::create(
      MachineDemandThresholdProfile{8.0F, 2.0F});
  auto second_unit = DemandThresholdEvaluator::create(
      MachineDemandThresholdProfile{12.0F, 4.0F});

  TEST_ASSERT_TRUE(first_unit.has_value());
  TEST_ASSERT_TRUE(second_unit.has_value());
  TEST_ASSERT_TRUE(first_unit->evaluate(8.0F) == DemandTransition::started);
  TEST_ASSERT_TRUE(second_unit->evaluate(8.0F) == DemandTransition::none);
  TEST_ASSERT_TRUE(first_unit->demand_active());
  TEST_ASSERT_FALSE(second_unit->demand_active());
}
