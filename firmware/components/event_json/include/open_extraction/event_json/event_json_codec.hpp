#pragma once

#include <optional>
#include <string>

#include "open_extraction/event/event.hpp"

namespace open_extraction::event_json {

class EventJsonCodec {
public:
  [[nodiscard]] static std::optional<std::string>
  serialize(const event::Event &event);
};

} // namespace open_extraction::event_json
