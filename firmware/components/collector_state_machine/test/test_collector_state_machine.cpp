#include <vector>

#include "unity.h"

#include "open_extraction/collector_state_machine/collector_state_machine.hpp"

namespace {

using open_extraction::collector_state_machine::CollectorState;
using open_extraction::collector_state_machine::CollectorStateMachine;
using open_extraction::collector_state_machine::Transition;

void assert_state(CollectorState expected, const CollectorStateMachine &m) {
  TEST_ASSERT_EQUAL_INT(static_cast<int>(expected),
                        static_cast<int>(m.state()));
}

CollectorStateMachine running_machine() {
  CollectorStateMachine machine;
  TEST_ASSERT_TRUE(machine.request_start());
  TEST_ASSERT_TRUE(machine.confirm_started());
  return machine;
}

} // namespace

TEST_CASE("a collector state machine starts stopped",
          "[collector_state_machine]") {
  assert_state(CollectorState::stopped, CollectorStateMachine());
}

TEST_CASE("a collector completes a start and stop lifecycle",
          "[collector_state_machine]") {
  CollectorStateMachine machine;

  TEST_ASSERT_TRUE(machine.request_start());
  assert_state(CollectorState::starting, machine);
  TEST_ASSERT_TRUE(machine.confirm_started());
  assert_state(CollectorState::running, machine);
  TEST_ASSERT_TRUE(machine.request_stop());
  assert_state(CollectorState::stopping, machine);
  TEST_ASSERT_TRUE(machine.confirm_stopped());
  assert_state(CollectorState::stopped, machine);
}

TEST_CASE("invalid transitions are rejected without changing state",
          "[collector_state_machine]") {
  CollectorStateMachine stopped;
  TEST_ASSERT_FALSE(stopped.confirm_started());
  TEST_ASSERT_FALSE(stopped.request_stop());
  TEST_ASSERT_FALSE(stopped.confirm_stopped());
  TEST_ASSERT_FALSE(stopped.recover());
  assert_state(CollectorState::stopped, stopped);

  CollectorStateMachine starting;
  TEST_ASSERT_TRUE(starting.request_start());
  TEST_ASSERT_FALSE(starting.request_start());
  TEST_ASSERT_FALSE(starting.request_stop());
  TEST_ASSERT_FALSE(starting.confirm_stopped());
  assert_state(CollectorState::starting, starting);

  auto running = running_machine();
  TEST_ASSERT_FALSE(running.request_start());
  TEST_ASSERT_FALSE(running.confirm_started());
  TEST_ASSERT_FALSE(running.confirm_stopped());
  assert_state(CollectorState::running, running);

  auto stopping = running_machine();
  TEST_ASSERT_TRUE(stopping.request_stop());
  TEST_ASSERT_FALSE(stopping.request_start());
  TEST_ASSERT_FALSE(stopping.request_stop());
  TEST_ASSERT_FALSE(stopping.confirm_started());
  assert_state(CollectorState::stopping, stopping);
}

TEST_CASE("a fault is accepted from every other state and blocks operation",
          "[collector_state_machine]") {
  CollectorStateMachine stopped;
  TEST_ASSERT_TRUE(stopped.fault());
  assert_state(CollectorState::faulted, stopped);
  TEST_ASSERT_FALSE(stopped.fault());
  TEST_ASSERT_FALSE(stopped.request_start());
  TEST_ASSERT_FALSE(stopped.confirm_started());
  TEST_ASSERT_FALSE(stopped.request_stop());
  TEST_ASSERT_FALSE(stopped.confirm_stopped());

  CollectorStateMachine starting;
  TEST_ASSERT_TRUE(starting.request_start());
  TEST_ASSERT_TRUE(starting.fault());

  auto running = running_machine();
  TEST_ASSERT_TRUE(running.fault());

  auto stopping = running_machine();
  TEST_ASSERT_TRUE(stopping.request_stop());
  TEST_ASSERT_TRUE(stopping.fault());
  assert_state(CollectorState::faulted, stopping);
}

TEST_CASE("a faulted collector recovers to stopped",
          "[collector_state_machine]") {
  CollectorStateMachine machine;
  TEST_ASSERT_TRUE(machine.fault());

  TEST_ASSERT_TRUE(machine.recover());
  assert_state(CollectorState::stopped, machine);
  TEST_ASSERT_TRUE(machine.request_start());
}

TEST_CASE("accepted transitions are reported to the observer",
          "[collector_state_machine]") {
  std::vector<Transition> transitions;
  CollectorStateMachine machine(
      [&transitions](const Transition &t) { transitions.push_back(t); });

  TEST_ASSERT_TRUE(machine.request_start());
  TEST_ASSERT_TRUE(machine.confirm_started());
  TEST_ASSERT_FALSE(machine.request_start());
  TEST_ASSERT_TRUE(machine.fault());

  TEST_ASSERT_EQUAL_UINT32(3, transitions.size());
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorState::stopped),
                        static_cast<int>(transitions[0].from));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorState::starting),
                        static_cast<int>(transitions[0].to));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorState::running),
                        static_cast<int>(transitions[1].to));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorState::running),
                        static_cast<int>(transitions[2].from));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorState::faulted),
                        static_cast<int>(transitions[2].to));
}
