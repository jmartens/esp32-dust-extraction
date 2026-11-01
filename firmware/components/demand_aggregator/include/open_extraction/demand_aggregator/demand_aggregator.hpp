#pragma once

#include <cstddef>
#include <functional>
#include <mutex>
#include <set>
#include <string>
#include <string_view>

namespace open_extraction::demand_aggregator {

// Aggregates demand across all active machine Units (ADR-0014). A Unit is
// identified by a stable id; starts and stops are idempotent per Unit, and
// aggregate demand exists while at least one Unit is active. All operations
// are thread-safe.
class DemandAggregator {
public:
  // Returns true when the Unit was newly activated, false for a duplicate.
  bool start(std::string_view unit_id);

  // Returns true when the Unit was active and is now stopped, false when it
  // was not active (duplicate or unknown stop).
  bool stop(std::string_view unit_id);

  [[nodiscard]] bool has_demand() const;
  [[nodiscard]] std::size_t active_count() const;
  [[nodiscard]] bool is_active(std::string_view unit_id) const;

private:
  mutable std::mutex mutex_;
  std::set<std::string, std::less<>> active_;
};

} // namespace open_extraction::demand_aggregator
