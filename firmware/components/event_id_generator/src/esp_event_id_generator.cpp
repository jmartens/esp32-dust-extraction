#include "open_extraction/event_id_generator/esp_event_id_generator.hpp"

#include <array>
#include <cstdint>

#include "esp_random.h"

namespace open_extraction::event_id_generator {

event::EventId EspEventIdGenerator::generate() {
  std::array<std::uint8_t, 16> bytes{};
  esp_fill_random(bytes.data(), bytes.size());

  bytes[6] = static_cast<std::uint8_t>((bytes[6] & 0x0FU) | 0x40U);
  bytes[8] = static_cast<std::uint8_t>((bytes[8] & 0x3FU) | 0x80U);

  return event::EventId::from_bytes(bytes);
}

} // namespace open_extraction::event_id_generator
