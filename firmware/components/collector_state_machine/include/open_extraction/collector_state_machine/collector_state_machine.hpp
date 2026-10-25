#pragma once

#include <functional>

#include "collector_unit/collector_unit.hpp"

namespace open_extraction::collector_state_machine {

using CollectorState = collector_unit::CollectorState;

struct Transition {
  CollectorState from;
  CollectorState to;
};

// Observers are notified after every accepted transition.
using TransitionObserver = std::function<void(const Transition &)>;

// Each operation returns false and leaves the state unchanged when the
// transition is not permitted from the current state.
class CollectorStateMachine {
public:
  explicit CollectorStateMachine(TransitionObserver observer = {});

  [[nodiscard]] CollectorState state() const noexcept;

  [[nodiscard]] bool request_start();
  [[nodiscard]] bool confirm_started();
  [[nodiscard]] bool request_stop();
  [[nodiscard]] bool confirm_stopped();
  [[nodiscard]] bool fault();
  [[nodiscard]] bool recover();

private:
  [[nodiscard]] bool transition(CollectorState expected, CollectorState next);

  CollectorState state_ = CollectorState::stopped;
  TransitionObserver observer_;
};

} // namespace open_extraction::collector_state_machine
