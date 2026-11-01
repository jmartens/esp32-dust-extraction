#include "open_extraction/demand_aggregator/demand_aggregator.hpp"

namespace open_extraction::demand_aggregator {

bool DemandAggregator::start(std::string_view unit_id) {
  const std::lock_guard lock(mutex_);
  return active_.emplace(unit_id).second;
}

bool DemandAggregator::stop(std::string_view unit_id) {
  const std::lock_guard lock(mutex_);
  const auto it = active_.find(unit_id);
  if (it == active_.end()) {
    return false;
  }
  active_.erase(it);
  return true;
}

bool DemandAggregator::has_demand() const {
  const std::lock_guard lock(mutex_);
  return !active_.empty();
}

std::size_t DemandAggregator::active_count() const {
  const std::lock_guard lock(mutex_);
  return active_.size();
}

bool DemandAggregator::is_active(std::string_view unit_id) const {
  const std::lock_guard lock(mutex_);
  return active_.find(unit_id) != active_.end();
}

} // namespace open_extraction::demand_aggregator
