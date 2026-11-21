#include "open_extraction/collector_service/collector_service.hpp"

#include <algorithm>
#include <string>
#include <utility>

#include "open_extraction/collector_commands/start_collector.hpp"
#include "open_extraction/collector_commands/stop_collector.hpp"
#include "open_extraction/collector_events/collector_fault.hpp"

namespace open_extraction::collector_service {

using collector_unit::CollectorState;

CollectorService::CollectorService(
    command_dispatcher::CommandDispatcher &dispatcher,
    event_dispatcher::EventDispatcher &event_dispatcher,
    const collector_unit::ICollectorUnit &unit, event::DeviceUid device_uid,
    event::EventIdGenerator &id_generator, Clock clock,
    std::chrono::milliseconds start_delay, std::chrono::milliseconds run_on)
    : dispatcher_(dispatcher), event_dispatcher_(event_dispatcher), unit_(unit),
      device_uid_(std::move(device_uid)), id_generator_(id_generator),
      clock_(std::move(clock)),
      start_delay_(std::max(start_delay, std::chrono::milliseconds::zero())),
      run_on_(std::max(run_on, std::chrono::milliseconds::zero())) {}

CollectorState CollectorService::state() const { return unit_.state(); }

collector_unit::CollectorMode CollectorService::mode() const { return mode_; }

std::string CollectorService::device_uid() const {
  return std::string(device_uid_.value());
}

bool CollectorService::demand_active() const { return demand_active_; }

bool CollectorService::demand_suppressed() const {
  return mode_ == collector_unit::CollectorMode::forced_off && demand_active_;
}

bool CollectorService::request_start() {
  if (mode_ == collector_unit::CollectorMode::forced_off) {
    return false;
  }
  if (unit_.state() != CollectorState::stopped) {
    return false;
  }
  return dispatch_command<collector_commands::StartCollector>();
}

bool CollectorService::request_stop() {
  if (mode_ == collector_unit::CollectorMode::forced_on) {
    return false;
  }
  if (unit_.state() != CollectorState::running) {
    return false;
  }
  return dispatch_command<collector_commands::StopCollector>();
}

bool CollectorService::request_mode(collector_unit::CollectorMode requested) {
  using collector_unit::CollectorMode;
  if (requested != CollectorMode::automatic &&
      requested != CollectorMode::forced_on &&
      requested != CollectorMode::forced_off) {
    return false;
  }

  const CollectorMode previous_mode = mode_;
  const auto previous_pending_demand_since = pending_demand_since_;
  const auto previous_run_on_since = run_on_since_;
  const bool changed = mode_ != requested;
  if (changed) {
    mode_ = requested;
    pending_demand_since_.reset();
    run_on_since_.reset();
  }

  // Returning to AUTO with existing demand is a mode transition, not new
  // demand, so it deliberately bypasses the start delay.
  if (reconcile(changed && requested == CollectorMode::automatic &&
                demand_active_)) {
    return true;
  }
  if (changed) {
    mode_ = previous_mode;
    pending_demand_since_ = previous_pending_demand_since;
    run_on_since_ = previous_run_on_since;
  }
  return false;
}

bool CollectorService::update_demand(bool active) {
  if (demand_active_ != active) {
    demand_active_ = active;
    if (active && mode_ == collector_unit::CollectorMode::automatic) {
      pending_demand_since_ = clock_();
    } else if (!active) {
      pending_demand_since_.reset();
    }
  }
  return reconcile();
}

bool CollectorService::process() { return reconcile(); }

bool CollectorService::reconcile(bool automatic_start_immediate) {
  using collector_unit::CollectorMode;

  if (mode_ != CollectorMode::automatic || demand_active_ ||
      unit_.state() != CollectorState::running) {
    run_on_since_.reset();
  }

  if (mode_ == CollectorMode::forced_on) {
    return unit_.state() == CollectorState::stopped ? request_start() : true;
  }
  if (mode_ == CollectorMode::forced_off) {
    return unit_.state() == CollectorState::running ? request_stop() : true;
  }

  if (!demand_active_) {
    pending_demand_since_.reset();
    if (unit_.state() != CollectorState::running) {
      return true;
    }
    if (!run_on_since_.has_value()) {
      run_on_since_ = clock_();
    }
    if (clock_() - *run_on_since_ < run_on_) {
      return true;
    }
    if (request_stop()) {
      run_on_since_.reset();
      return true;
    }
    return false;
  }

  if (automatic_start_immediate) {
    pending_demand_since_.reset();
    return unit_.state() == CollectorState::stopped ? request_start() : true;
  }

  if (!pending_demand_since_.has_value()) {
    return unit_.state() == CollectorState::stopped ? request_start() : true;
  }

  if (clock_() - *pending_demand_since_ < start_delay_) {
    return true;
  }

  if (unit_.state() != CollectorState::stopped) {
    pending_demand_since_.reset();
    return true;
  }
  if (request_start()) {
    pending_demand_since_.reset();
    return true;
  }
  return false;
}

void CollectorService::report_fault(std::string reason) {
  const collector_events::CollectorFault fault(
      id_generator_.generate(), clock_(), device_uid_, std::move(reason));
  event_dispatcher_.publish(fault);
}

template <typename CommandT> bool CollectorService::dispatch_command() const {
  const auto id = command::CommandId::from_string(
      std::string(id_generator_.generate().value()));
  if (!id) {
    return false;
  }
  const CommandT command(*id, device_uid_, clock_());
  return dispatcher_.dispatch(command) ==
         command_dispatcher::DispatchResult::delivered;
}

} // namespace open_extraction::collector_service
