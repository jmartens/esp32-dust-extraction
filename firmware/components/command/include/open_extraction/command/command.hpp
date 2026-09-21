#pragma once

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "open_extraction/event/event.hpp"

namespace open_extraction::command {

using Timestamp = event::Timestamp;
using DeviceUid = event::DeviceUid;
using SerializedPayload = std::vector<std::uint8_t>;

class CommandId {
public:
  [[nodiscard]] static CommandId
  from_bytes(const std::array<std::uint8_t, 16> &bytes);
  [[nodiscard]] static std::optional<CommandId> from_string(std::string value);

  [[nodiscard]] std::string_view value() const noexcept;

private:
  explicit CommandId(event::EventId id);

  event::EventId id_;
};

class CommandType {
public:
  [[nodiscard]] static std::optional<CommandType> from_string(std::string value);

  [[nodiscard]] std::string_view value() const noexcept;

private:
  explicit CommandType(std::string value);

  std::string value_;
};

struct CommandMetadata {
  CommandId id;
  CommandType type;
  DeviceUid target;
  Timestamp timestamp;
};

class Command {
public:
  Command(const Command &) = default;
  Command(Command &&) noexcept = default;
  Command &operator=(const Command &) = default;
  Command &operator=(Command &&) noexcept = default;
  virtual ~Command() = default;

  [[nodiscard]] std::string_view command_id() const noexcept;
  [[nodiscard]] std::string_view command_type() const noexcept;
  [[nodiscard]] std::string_view target() const noexcept;
  [[nodiscard]] Timestamp timestamp() const noexcept;

  // Transport adapters encode the command payload in their protocol format.
  [[nodiscard]] virtual SerializedPayload serialize_payload() const = 0;

protected:
  explicit Command(CommandMetadata metadata);

private:
  CommandMetadata metadata_;
};

} // namespace open_extraction::command
