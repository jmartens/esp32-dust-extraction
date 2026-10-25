#include "open_extraction/collector_state_machine/collector_state_machine.hpp"

#include <utility>

namespace open_extraction::collector_state_machine {

CollectorStateMachine::CollectorStateMachine(TransitionObserver observer)
    : observer_(std::move(observer)) {}

CollectorState CollectorStateMachine::state() const noexcept { return state_; }

bool CollectorStateMachine::request_start() {
  return transition(CollectorState::stopped, CollectorState::starting);
}

bool CollectorStateMachine::confirm_started() {
  return transition(CollectorState::starting, CollectorState::running);
}

bool CollectorStateMachine::request_stop() {
  return transition(CollectorState::running, CollectorState::stopping);
}

bool CollectorStateMachine::confirm_stopped() {
  return transition(CollectorState::stopping, CollectorState::stopped);
}

bool CollectorStateMachine::fault() {
  if (state_ == CollectorState::faulted) {
    return false;
  }

  return transition(state_, CollectorState::faulted);
}

bool CollectorStateMachine::recover() {
  return transition(CollectorState::faulted, CollectorState::stopped);
}

bool CollectorStateMachine::transition(CollectorState expected,
                                       CollectorState next) {
  if (state_ != expected) {
    return false;
  }

  const Transition accepted{.from = state_, .to = next};
  state_ = next;
  if (observer_) {
    observer_(accepted);
  }
  return true;
}

} // namespace open_extraction::collector_state_machine
