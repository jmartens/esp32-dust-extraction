#include "unity.h"

#include "open_extraction/event_id_generator/esp_event_id_generator.hpp"

TEST_CASE("the ESP event ID generator creates UUID version 4 identifiers",
          "[event_id_generator]") {
  open_extraction::event_id_generator::EspEventIdGenerator generator;

  const auto event_id = generator.generate().value();

  TEST_ASSERT_EQUAL_UINT8('4', event_id[14]);
  TEST_ASSERT_TRUE(event_id[19] == '8' || event_id[19] == '9' ||
                   event_id[19] == 'a' || event_id[19] == 'b');
}
