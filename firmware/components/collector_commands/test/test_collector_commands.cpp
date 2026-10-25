#include <chrono>
#include <type_traits>

#include "unity.h"

#include "open_extraction/collector_commands/start_collector.hpp"
#include "open_extraction/collector_commands/stop_collector.hpp"

namespace {

using open_extraction::collector_commands::StartCollector;
using open_extraction::collector_commands::StopCollector;
using open_extraction::command::CommandId;
using open_extraction::command::DeviceUid;
using open_extraction::command::Timestamp;

template <typename CollectorCommand>
void assert_metadata(const CollectorCommand &command,
                     const char *command_type) {
  TEST_ASSERT_EQUAL_STRING("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742",
                           command.command_id().data());
  TEST_ASSERT_EQUAL_STRING(command_type, command.command_type().data());
  TEST_ASSERT_EQUAL_STRING("COLLECTOR-58BF25A77109", command.target().data());
  TEST_ASSERT_EQUAL_INT64(1'726'258'400'123,
                          command.timestamp().time_since_epoch().count());
}

CommandId command_id() {
  return CommandId::from_string("ea290ba8-5255-4c7f-9fc6-9a1b94d5a742")
      .value();
}

DeviceUid target() {
  return DeviceUid::from_string("COLLECTOR-58BF25A77109").value();
}

Timestamp timestamp() {
  return Timestamp(std::chrono::milliseconds(1'726'258'400'123));
}

} // namespace

TEST_CASE("a start collector command preserves immutable metadata",
          "[collector_commands]") {
  const StartCollector command(command_id(), target(), timestamp());

  assert_metadata(command, "collector.start");
  TEST_ASSERT_EQUAL_STRING("collector.start",
                           StartCollector::kCommandType.data());
  TEST_ASSERT_FALSE(std::is_copy_assignable_v<StartCollector>);
  TEST_ASSERT_FALSE(std::is_move_assignable_v<StartCollector>);
}

TEST_CASE("a stop collector command preserves immutable metadata",
          "[collector_commands]") {
  const StopCollector command(command_id(), target(), timestamp());

  assert_metadata(command, "collector.stop");
  TEST_ASSERT_EQUAL_STRING("collector.stop", StopCollector::kCommandType.data());
  TEST_ASSERT_FALSE(std::is_copy_assignable_v<StopCollector>);
  TEST_ASSERT_FALSE(std::is_move_assignable_v<StopCollector>);
}

TEST_CASE("collector commands serialize through the command boundary",
          "[collector_commands]") {
  const StartCollector start(command_id(), target(), timestamp());
  const StopCollector stop(command_id(), target(), timestamp());
  const open_extraction::command::Command &start_command = start;
  const open_extraction::command::Command &stop_command = stop;

  TEST_ASSERT_TRUE(start_command.serialize_payload().empty());
  TEST_ASSERT_TRUE(stop_command.serialize_payload().empty());
}

TEST_CASE("collector commands can be copied", "[collector_commands]") {
  const StartCollector original(command_id(), target(), timestamp());
  const StartCollector copy = original;

  assert_metadata(copy, "collector.start");
}
