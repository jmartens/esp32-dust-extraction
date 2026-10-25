#include "open_extraction/collector_service/collector_service.hpp"

#include <string>
#include <utility>

#include "open_extraction/collector_commands/start_collector.hpp"
#include "open_extraction/collector_commands/stop_collector.hpp"

namespace open_extraction::collector_service {

using collector_unit::CollectorState;

CollectorService::CollectorService(
    command_dispatcher::CommandDispatcher &dispatcher,
    const collector_unit::ICollectorUnit &unit, event::DeviceUid device_uid,
    event::EventIdGenerator &id_generator, Clock clock)
    : dispatcher_(dispatcher), unit_(unit), device_uid_(std::move(device_uid)),
      id_generator_(id_generator), clock_(std::move(clock)) {}

CollectorState CollectorService::state() const { return unit_.state(); }

std::string CollectorService::device_uid() const {
  return std::string(device_uid_.value());
}

bool CollectorService::request_start() {
  if (unit_.state() != CollectorState::stopped) {
    return false;
  }
  return dispatch_command<collector_commands::StartCollector>();
}

bool CollectorService::request_stop() {
  if (unit_.state() != CollectorState::running) {
    return false;
  }
  return dispatch_command<collector_commands::StopCollector>();
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
