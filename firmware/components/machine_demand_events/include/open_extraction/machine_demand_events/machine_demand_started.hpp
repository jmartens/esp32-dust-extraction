#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "open_extraction/event/event.hpp"

namespace open_extraction::machine_demand_events {

class MachineDemandStarted final : public event::Event {
public:
  static constexpr std::string_view kEventType{"machine_demand.started"};

  MachineDemandStarted(event::EventId id, event::Timestamp occurred_at,
                       event::DeviceUid originating_device, std::string unit_id)
      : Event({
            .id = std::move(id),
            .type =
                event::EventType::from_string(std::string(kEventType)).value(),
            .occurred_at = occurred_at,
            .originating_device = std::move(originating_device),
            .unit_id = std::move(unit_id),
        }) {}

  MachineDemandStarted(const MachineDemandStarted &) = default;
  MachineDemandStarted(MachineDemandStarted &&) noexcept = default;
  MachineDemandStarted &operator=(const MachineDemandStarted &) = delete;
  MachineDemandStarted &operator=(MachineDemandStarted &&) noexcept = delete;
  ~MachineDemandStarted() override = default;
};

} // namespace open_extraction::machine_demand_events
