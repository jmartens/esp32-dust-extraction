#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "open_extraction/command/command.hpp"

namespace open_extraction::collector_commands {

class StopCollector final : public command::Command {
public:
  static constexpr std::string_view kCommandType{"collector.stop"};

  StopCollector(command::CommandId id, command::DeviceUid target,
                command::Timestamp timestamp)
      : Command({
            .id = std::move(id),
            .type = command::CommandType::from_string(std::string(kCommandType))
                        .value(),
            .target = std::move(target),
            .timestamp = timestamp,
        }) {}

  StopCollector(const StopCollector &) = default;
  StopCollector(StopCollector &&) noexcept = default;
  StopCollector &operator=(const StopCollector &) = delete;
  StopCollector &operator=(StopCollector &&) noexcept = delete;
  ~StopCollector() override = default;

  // The command type carries all intent; there are no parameters.
  [[nodiscard]] command::SerializedPayload serialize_payload() const override {
    return {};
  }
};

} // namespace open_extraction::collector_commands
