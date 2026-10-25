#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "open_extraction/command/command.hpp"

namespace open_extraction::collector_commands {

class StartCollector final : public command::Command {
public:
  static constexpr std::string_view kCommandType{"collector.start"};

  StartCollector(command::CommandId id, command::DeviceUid target,
                 command::Timestamp timestamp)
      : Command({
            .id = std::move(id),
            .type = command::CommandType::from_string(
                        std::string(kCommandType))
                        .value(),
            .target = std::move(target),
            .timestamp = timestamp,
        }) {}

  StartCollector(const StartCollector &) = default;
  StartCollector(StartCollector &&) noexcept = default;
  StartCollector &operator=(const StartCollector &) = delete;
  StartCollector &operator=(StartCollector &&) noexcept = delete;
  ~StartCollector() override = default;

  // The command type carries all intent; there are no parameters.
  [[nodiscard]] command::SerializedPayload serialize_payload() const override {
    return {};
  }
};

} // namespace open_extraction::collector_commands
