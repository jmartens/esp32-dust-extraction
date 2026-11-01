#include <string>
#include <thread>
#include <vector>

#include "unity.h"

#include "open_extraction/demand_aggregator/demand_aggregator.hpp"

using open_extraction::demand_aggregator::DemandAggregator;

TEST_CASE("a new aggregator reports no demand", "[demand_aggregator]") {
  const DemandAggregator aggregator;

  TEST_ASSERT_FALSE(aggregator.has_demand());
  TEST_ASSERT_EQUAL_UINT32(0, aggregator.active_count());
}

TEST_CASE("a single unit start and stop toggles demand",
          "[demand_aggregator]") {
  DemandAggregator aggregator;

  TEST_ASSERT_TRUE(aggregator.start("saw"));
  TEST_ASSERT_TRUE(aggregator.has_demand());
  TEST_ASSERT_TRUE(aggregator.is_active("saw"));
  TEST_ASSERT_TRUE(aggregator.stop("saw"));
  TEST_ASSERT_FALSE(aggregator.has_demand());
  TEST_ASSERT_FALSE(aggregator.is_active("saw"));
}

TEST_CASE("multiple active demands keep aggregate demand until all stop",
          "[demand_aggregator]") {
  DemandAggregator aggregator;

  TEST_ASSERT_TRUE(aggregator.start("saw"));
  TEST_ASSERT_TRUE(aggregator.start("sander"));
  TEST_ASSERT_TRUE(aggregator.start("router"));
  TEST_ASSERT_EQUAL_UINT32(3, aggregator.active_count());

  TEST_ASSERT_TRUE(aggregator.stop("saw"));
  TEST_ASSERT_TRUE(aggregator.has_demand());
  TEST_ASSERT_TRUE(aggregator.stop("router"));
  TEST_ASSERT_TRUE(aggregator.has_demand());
  TEST_ASSERT_TRUE(aggregator.is_active("sander"));
  TEST_ASSERT_TRUE(aggregator.stop("sander"));
  TEST_ASSERT_FALSE(aggregator.has_demand());
}

TEST_CASE("duplicate starts are idempotent", "[demand_aggregator]") {
  DemandAggregator aggregator;

  TEST_ASSERT_TRUE(aggregator.start("saw"));
  TEST_ASSERT_FALSE(aggregator.start("saw"));
  TEST_ASSERT_EQUAL_UINT32(1, aggregator.active_count());

  TEST_ASSERT_TRUE(aggregator.stop("saw"));
  TEST_ASSERT_FALSE(aggregator.has_demand());
}

TEST_CASE("duplicate and unknown stops are idempotent",
          "[demand_aggregator]") {
  DemandAggregator aggregator;

  TEST_ASSERT_FALSE(aggregator.stop("unknown"));
  TEST_ASSERT_TRUE(aggregator.start("saw"));
  TEST_ASSERT_TRUE(aggregator.start("sander"));
  TEST_ASSERT_TRUE(aggregator.stop("saw"));
  TEST_ASSERT_FALSE(aggregator.stop("saw"));

  TEST_ASSERT_TRUE(aggregator.has_demand());
  TEST_ASSERT_EQUAL_UINT32(1, aggregator.active_count());
}

TEST_CASE("a unit can restart after stopping", "[demand_aggregator]") {
  DemandAggregator aggregator;

  TEST_ASSERT_TRUE(aggregator.start("saw"));
  TEST_ASSERT_TRUE(aggregator.stop("saw"));
  TEST_ASSERT_TRUE(aggregator.start("saw"));
  TEST_ASSERT_TRUE(aggregator.has_demand());
}

TEST_CASE("concurrent starts and stops never lose active demand",
          "[demand_aggregator]") {
  constexpr int unit_count = 16;
  constexpr int iterations = 200;
  DemandAggregator aggregator;

  // A permanently active unit must survive all concurrent churn.
  TEST_ASSERT_TRUE(aggregator.start("persistent"));

  std::vector<std::thread> threads;
  for (int i = 0; i < unit_count; ++i) {
    threads.emplace_back([&aggregator, i] {
      const std::string id = "unit-" + std::to_string(i);
      for (int n = 0; n < iterations; ++n) {
        (void)aggregator.start(id);
        (void)aggregator.start(id);
        (void)aggregator.stop(id);
        (void)aggregator.stop(id);
      }
      (void)aggregator.start(id);
    });
  }
  for (auto &thread : threads) {
    thread.join();
  }

  TEST_ASSERT_EQUAL_UINT32(unit_count + 1, aggregator.active_count());

  threads.clear();
  for (int i = 0; i < unit_count; ++i) {
    threads.emplace_back([&aggregator, i] {
      (void)aggregator.stop("unit-" + std::to_string(i));
    });
  }
  for (auto &thread : threads) {
    thread.join();
  }

  TEST_ASSERT_TRUE(aggregator.has_demand());
  TEST_ASSERT_TRUE(aggregator.is_active("persistent"));
  TEST_ASSERT_TRUE(aggregator.stop("persistent"));
  TEST_ASSERT_FALSE(aggregator.has_demand());
}
