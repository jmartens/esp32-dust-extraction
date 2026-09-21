#pragma once

#include "open_extraction/event/event.hpp"

namespace open_extraction::event_id_generator {

class EspEventIdGenerator final : public event::EventIdGenerator {
public:
  [[nodiscard]] event::EventId generate() override;
};

} // namespace open_extraction::event_id_generator
