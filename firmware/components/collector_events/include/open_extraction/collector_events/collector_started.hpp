#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "open_extraction/event/event.hpp"

namespace open_extraction::collector_events {

class CollectorStarted final : public event::Event {
public:
  static constexpr std::string_view kEventType{"collector.started"};

  CollectorStarted(event::EventId id, event::Timestamp occurred_at,
                   event::DeviceUid originating_device)
      : Event({
            .id = std::move(id),
            .type =
                event::EventType::from_string(std::string(kEventType)).value(),
            .occurred_at = occurred_at,
            .originating_device = std::move(originating_device),
        }) {}

  CollectorStarted(const CollectorStarted &) = default;
  CollectorStarted(CollectorStarted &&) noexcept = default;
  CollectorStarted &operator=(const CollectorStarted &) = delete;
  CollectorStarted &operator=(CollectorStarted &&) noexcept = delete;
  ~CollectorStarted() override = default;
};

} // namespace open_extraction::collector_events
