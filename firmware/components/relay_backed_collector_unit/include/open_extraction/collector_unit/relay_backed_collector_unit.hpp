#pragma once

#include <array>
#include <span>

#include "collector_unit/collector_unit.hpp"
#include "open_extraction/collector_state_machine/collector_state_machine.hpp"
#include "relay/relay_driver.hpp"

namespace open_extraction::collector_unit {

class RelayBackedCollectorUnit final : public ICollectorUnit {
public:
  explicit RelayBackedCollectorUnit(relay::RelayDriver &relay);

  bool start() override;
  bool stop() override;

  [[nodiscard]] CollectorState state() const override;
  [[nodiscard]] CollectorDiagnostics diagnostics() const override;
  [[nodiscard]] std::span<const CollectorCapability>
  capabilities() const override;

private:
  static constexpr std::array kCapabilities{CollectorCapability::start_stop};

  relay::RelayDriver &relay_;
  collector_state_machine::CollectorStateMachine state_machine_;
  CollectorDiagnostics diagnostics_{};
};

} // namespace open_extraction::collector_unit
