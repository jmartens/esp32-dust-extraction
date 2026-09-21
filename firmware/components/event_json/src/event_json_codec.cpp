#include "open_extraction/event_json/event_json_codec.hpp"

#include <memory>

#include "cJSON.h"

namespace open_extraction::event_json {
namespace {

using JsonPointer = std::unique_ptr<cJSON, decltype(&cJSON_Delete)>;

} // namespace

std::optional<std::string>
EventJsonCodec::serialize(const event::Event &event) {
  JsonPointer json(cJSON_CreateObject(), cJSON_Delete);
  if (!json) {
    return std::nullopt;
  }

  const auto timestamp = event.occurred_at().time_since_epoch().count();
  if (cJSON_AddStringToObject(json.get(), "event_id",
                              event.event_id().data()) == nullptr ||
      cJSON_AddStringToObject(json.get(), "event_type",
                              event.event_type().data()) == nullptr ||
      cJSON_AddNumberToObject(json.get(), "occurred_at_unix_ms",
                              static_cast<double>(timestamp)) == nullptr ||
      cJSON_AddStringToObject(json.get(), "originating_device",
                              event.originating_device().data()) == nullptr) {
    return std::nullopt;
  }

  std::unique_ptr<char, decltype(&cJSON_free)> serialized(
      cJSON_PrintUnformatted(json.get()), cJSON_free);
  if (!serialized) {
    return std::nullopt;
  }

  return std::string(serialized.get());
}

} // namespace open_extraction::event_json
