#include <array>
#include <chrono>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "unity.h"

#include "open_extraction/command/command.hpp"

namespace {

using open_extraction::command::Command;
using open_extraction::command::CommandId;
using open_extraction::command::CommandMetadata;
using open_extraction::command::CommandType;
using open_extraction::command::DeviceUid;
using open_extraction::command::SerializedPayload;
using open_extraction::command::Timestamp;

class TestCommand final : public Command {
public:
  explicit TestCommand(CommandMetadata metadata) : Command(std::move(metadata)) {}

  [[nodiscard]] SerializedPayload serialize_payload() const override {
    return {0x01, 0x02};
  }
};

CommandMetadata valid_metadata() {
  const auto command_id =
      CommandId::from_string("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742").value();
  const auto command_type = CommandType::from_string("gate.open").value();
  const auto target =
      DeviceUid::from_string("GATE-58BF25A77109").value();

  return {
      .id = command_id,
      .type = command_type,
      .target = target,
      .timestamp = Timestamp(std::chrono::milliseconds(1'726'258'400'123)),
  };
}

static_assert(std::is_abstract_v<Command>);
static_assert(std::is_copy_constructible_v<TestCommand>);
static_assert(std::is_copy_assignable_v<TestCommand>);
static_assert(std::is_move_constructible_v<TestCommand>);
static_assert(std::is_move_assignable_v<TestCommand>);

} // namespace

TEST_CASE("a command exposes immutable metadata", "[command]") {
  const TestCommand command(valid_metadata());

  TEST_ASSERT_EQUAL_STRING("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742",
                           command.command_id().data());
  TEST_ASSERT_EQUAL_STRING("gate.open", command.command_type().data());
  TEST_ASSERT_EQUAL_STRING("GATE-58BF25A77109", command.target().data());
  TEST_ASSERT_EQUAL_INT64(1'726'258'400'123,
                          command.timestamp().time_since_epoch().count());
}

TEST_CASE("a command serializes its payload through its boundary", "[command]") {
  const TestCommand command(valid_metadata());
  constexpr std::array<std::uint8_t, 2> expected_payload = {0x01, 0x02};

  const auto payload = command.serialize_payload();

  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_payload.data(), payload.data(),
                                payload.size());
}

TEST_CASE("a command can be copied and moved", "[command]") {
  const TestCommand original(valid_metadata());
  const TestCommand copy = original;
  TestCommand moved = std::move(copy);

  TEST_ASSERT_EQUAL_STRING(original.command_id().data(), moved.command_id().data());
  TEST_ASSERT_EQUAL_STRING(original.command_type().data(),
                           moved.command_type().data());
  TEST_ASSERT_EQUAL_STRING(original.target().data(), moved.target().data());
  TEST_ASSERT_EQUAL_INT64(original.timestamp().time_since_epoch().count(),
                          moved.timestamp().time_since_epoch().count());
}

TEST_CASE("command metadata rejects malformed values", "[command]") {
  TEST_ASSERT_FALSE(CommandId::from_string("not-a-uuid").has_value());
  TEST_ASSERT_FALSE(CommandType::from_string("").has_value());
  TEST_ASSERT_FALSE(DeviceUid::from_string("gate-58BF25A77109").has_value());
}
