#include "open_extraction/command/command.hpp"

#include <utility>

namespace open_extraction::command {

CommandId CommandId::from_bytes(const std::array<std::uint8_t, 16> &bytes) {
  return CommandId(event::EventId::from_bytes(bytes));
}

std::optional<CommandId> CommandId::from_string(std::string value) {
  const auto event_id = event::EventId::from_string(std::move(value));
  if (!event_id.has_value()) {
    return std::nullopt;
  }

  return CommandId(*event_id);
}

CommandId::CommandId(event::EventId id) : id_(std::move(id)) {}

std::string_view CommandId::value() const noexcept { return id_.value(); }

std::optional<CommandType> CommandType::from_string(std::string value) {
  if (value.empty()) {
    return std::nullopt;
  }

  return CommandType(std::move(value));
}

CommandType::CommandType(std::string value) : value_(std::move(value)) {}

std::string_view CommandType::value() const noexcept { return value_; }

Command::Command(CommandMetadata metadata) : metadata_(std::move(metadata)) {}

std::string_view Command::command_id() const noexcept {
  return metadata_.id.value();
}

std::string_view Command::command_type() const noexcept {
  return metadata_.type.value();
}

std::string_view Command::target() const noexcept {
  return metadata_.target.value();
}

Timestamp Command::timestamp() const noexcept { return metadata_.timestamp; }

} // namespace open_extraction::command
