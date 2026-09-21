#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "open_extraction/event/event.hpp"

namespace open_extraction::collector_events {

class CollectorStopped final : public event::Event {
public:
  static constexpr std::string_view kEventType{"collector.stopped"};

  CollectorStopped(event::EventId id, event::Timestamp occurred_at,
                   event::DeviceUid originating_device)
      : Event({
            .id = std::move(id),
            .type =
                event::EventType::from_string(std::string(kEventType)).value(),
            .occurred_at = occurred_at,
            .originating_device = std::move(originating_device),
        }) {}

  CollectorStopped(const CollectorStopped &) = default;
  CollectorStopped(CollectorStopped &&) noexcept = default;
  CollectorStopped &operator=(const CollectorStopped &) = delete;
  CollectorStopped &operator=(CollectorStopped &&) noexcept = delete;
  ~CollectorStopped() override = default;
};

} // namespace open_extraction::collector_events
