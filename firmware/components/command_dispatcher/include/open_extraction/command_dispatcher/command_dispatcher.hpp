#pragma once

#include <functional>
#include <string>
#include <vector>

#include "open_extraction/command/command.hpp"

namespace open_extraction::command_dispatcher {

using CommandRecipient = std::function<bool(const command::Command &)>;

enum class DispatchResult {
  delivered,
  unknown_recipient,
  delivery_failed,
};

class CommandDispatcher {
public:
  [[nodiscard]] bool
  register_recipient(const command::DeviceUid &recipient,
                     CommandRecipient handler);
  [[nodiscard]] bool
  unregister_recipient(const command::DeviceUid &recipient);
  [[nodiscard]] DispatchResult dispatch(const command::Command &command) const;

private:
  struct Recipient {
    std::string uid;
    CommandRecipient handler;
  };

  std::vector<Recipient> recipients_;
};

} // namespace open_extraction::command_dispatcher
