#include "open_extraction/command_dispatcher/command_dispatcher.hpp"

#include <algorithm>
#include <string_view>
#include <utility>

namespace open_extraction::command_dispatcher {

bool CommandDispatcher::register_recipient(
    const command::DeviceUid &recipient, CommandRecipient handler) {
  if (!handler) {
    return false;
  }

  const std::string uid(recipient.value());
  const auto existing =
      std::find_if(recipients_.begin(), recipients_.end(),
                   [&uid](const Recipient &candidate) {
                     return candidate.uid == uid;
                   });
  if (existing != recipients_.end()) {
    return false;
  }

  recipients_.push_back({uid, std::move(handler)});
  return true;
}

bool CommandDispatcher::unregister_recipient(
    const command::DeviceUid &recipient) {
  const std::string_view uid = recipient.value();
  const auto existing =
      std::find_if(recipients_.begin(), recipients_.end(),
                   [uid](const Recipient &candidate) {
                     return candidate.uid == uid;
                   });
  if (existing == recipients_.end()) {
    return false;
  }

  recipients_.erase(existing);
  return true;
}

DispatchResult
CommandDispatcher::dispatch(const command::Command &command) const {
  const std::string_view target = command.target();
  const auto recipient =
      std::find_if(recipients_.begin(), recipients_.end(),
                   [target](const Recipient &candidate) {
                     return candidate.uid == target;
                   });
  if (recipient == recipients_.end()) {
    return DispatchResult::unknown_recipient;
  }

  const CommandRecipient handler = recipient->handler;
  return handler(command) ? DispatchResult::delivered
                          : DispatchResult::delivery_failed;
}

} // namespace open_extraction::command_dispatcher
