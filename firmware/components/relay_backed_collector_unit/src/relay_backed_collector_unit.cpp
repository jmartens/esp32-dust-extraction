#include "open_extraction/collector_unit/relay_backed_collector_unit.hpp"

namespace open_extraction::collector_unit {

RelayBackedCollectorUnit::RelayBackedCollectorUnit(relay::RelayDriver &relay)
    : relay_(relay) {
  relay_.off();
}

bool RelayBackedCollectorUnit::start() {
  if (!state_machine_.request_start()) {
    return false;
  }

  relay_.on();
  if (!state_machine_.confirm_started()) {
    relay_.off();
    (void)state_machine_.fault();
    return false;
  }

  ++diagnostics_.successful_starts;
  return true;
}

bool RelayBackedCollectorUnit::stop() {
  if (!state_machine_.request_stop()) {
    return false;
  }

  relay_.off();
  if (!state_machine_.confirm_stopped()) {
    (void)state_machine_.fault();
    return false;
  }

  ++diagnostics_.successful_stops;
  return true;
}

CollectorState RelayBackedCollectorUnit::state() const {
  return state_machine_.state();
}

CollectorDiagnostics RelayBackedCollectorUnit::diagnostics() const {
  return diagnostics_;
}

std::span<const CollectorCapability>
RelayBackedCollectorUnit::capabilities() const {
  return kCapabilities;
}

} // namespace open_extraction::collector_unit
