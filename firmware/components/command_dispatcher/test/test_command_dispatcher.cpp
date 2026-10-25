#include <chrono>
#include <utility>

#include "unity.h"

#include "open_extraction/command/command.hpp"
#include "open_extraction/command_dispatcher/command_dispatcher.hpp"

namespace {

using open_extraction::command::Command;
using open_extraction::command::CommandId;
using open_extraction::command::CommandMetadata;
using open_extraction::command::CommandType;
using open_extraction::command::DeviceUid;
using open_extraction::command::SerializedPayload;
using open_extraction::command::Timestamp;
using open_extraction::command_dispatcher::CommandDispatcher;
using open_extraction::command_dispatcher::DispatchResult;

class TestCommand final : public Command {
public:
  explicit TestCommand(CommandMetadata metadata)
      : Command(std::move(metadata)) {}

  [[nodiscard]] SerializedPayload serialize_payload() const override {
    return {0x01};
  }
};

CommandMetadata valid_metadata() {
  return {
      .id =
          CommandId::from_string("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742").value(),
      .type = CommandType::from_string("gate.open").value(),
      .target = DeviceUid::from_string("GATE-58BF25A77109").value(),
      .timestamp = Timestamp(std::chrono::milliseconds(1'726'258'400'123)),
  };
}

DeviceUid target() {
  return DeviceUid::from_string("GATE-58BF25A77109").value();
}

} // namespace

TEST_CASE("dispatches a command to its registered recipient",
          "[command_dispatcher]") {
  CommandDispatcher dispatcher;
  bool received = false;

  TEST_ASSERT_TRUE(dispatcher.register_recipient(
      target(), [&received](const Command &command) {
        received = true;
        return command.command_type() == "gate.open";
      }));

  const auto result = dispatcher.dispatch(TestCommand(valid_metadata()));

  TEST_ASSERT_EQUAL_INT(static_cast<int>(DispatchResult::delivered),
                        static_cast<int>(result));
  TEST_ASSERT_TRUE(received);
}

TEST_CASE("dispatches only to the recipient addressed by the command",
          "[command_dispatcher]") {
  CommandDispatcher dispatcher;
  int target_calls = 0;
  int other_calls = 0;

  TEST_ASSERT_TRUE(dispatcher.register_recipient(
      target(), [&target_calls](const Command &) {
        ++target_calls;
        return true;
      }));
  TEST_ASSERT_TRUE(dispatcher.register_recipient(
      DeviceUid::from_string("GATE-112233445566").value(),
      [&other_calls](const Command &) {
        ++other_calls;
        return true;
      }));

  (void)dispatcher.dispatch(TestCommand(valid_metadata()));

  TEST_ASSERT_EQUAL_INT(1, target_calls);
  TEST_ASSERT_EQUAL_INT(0, other_calls);
}

TEST_CASE("reports commands addressed to unknown recipients",
          "[command_dispatcher]") {
  CommandDispatcher dispatcher;

  const auto result = dispatcher.dispatch(TestCommand(valid_metadata()));

  TEST_ASSERT_EQUAL_INT(static_cast<int>(DispatchResult::unknown_recipient),
                        static_cast<int>(result));
}

TEST_CASE("reports recipient delivery failures", "[command_dispatcher]") {
  CommandDispatcher dispatcher;
  TEST_ASSERT_TRUE(dispatcher.register_recipient(
      target(), [](const Command &) { return false; }));

  const auto result = dispatcher.dispatch(TestCommand(valid_metadata()));

  TEST_ASSERT_EQUAL_INT(static_cast<int>(DispatchResult::delivery_failed),
                        static_cast<int>(result));
}

TEST_CASE("recipient registration rejects empty and duplicate handlers",
          "[command_dispatcher]") {
  CommandDispatcher dispatcher;

  TEST_ASSERT_FALSE(dispatcher.register_recipient(target(), {}));
  TEST_ASSERT_TRUE(dispatcher.register_recipient(
      target(), [](const Command &) { return true; }));
  TEST_ASSERT_FALSE(dispatcher.register_recipient(
      target(), [](const Command &) { return true; }));
}

TEST_CASE("unregistering a recipient stops command delivery",
          "[command_dispatcher]") {
  CommandDispatcher dispatcher;
  TEST_ASSERT_TRUE(dispatcher.register_recipient(
      target(), [](const Command &) { return true; }));

  TEST_ASSERT_TRUE(dispatcher.unregister_recipient(target()));
  TEST_ASSERT_FALSE(dispatcher.unregister_recipient(target()));
  TEST_ASSERT_EQUAL_INT(
      static_cast<int>(DispatchResult::unknown_recipient),
      static_cast<int>(dispatcher.dispatch(TestCommand(valid_metadata()))));
}
