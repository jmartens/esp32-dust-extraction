#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "open_extraction/event/event.hpp"

namespace open_extraction::collector_events {

class CollectorFault final : public event::Event {
public:
  static constexpr std::string_view kEventType{"collector.fault"};

  CollectorFault(event::EventId id, event::Timestamp occurred_at,
                 event::DeviceUid originating_device, std::string reason)
      : Event({
            .id = std::move(id),
            .type =
                event::EventType::from_string(std::string(kEventType)).value(),
            .occurred_at = occurred_at,
            .originating_device = std::move(originating_device),
        }),
        reason_(std::move(reason)) {}

  CollectorFault(const CollectorFault &) = default;
  CollectorFault(CollectorFault &&) noexcept = default;
  CollectorFault &operator=(const CollectorFault &) = delete;
  CollectorFault &operator=(CollectorFault &&) noexcept = delete;
  ~CollectorFault() override = default;

  [[nodiscard]] const std::string &reason() const noexcept { return reason_; }
  [[nodiscard]] std::optional<std::string_view>
  fault_reason() const noexcept override {
    return reason_;
  }

private:
  std::string reason_;
};

} // namespace open_extraction::collector_events
