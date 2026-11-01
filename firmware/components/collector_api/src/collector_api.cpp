#include "open_extraction/collector_api/collector_api.hpp"

namespace open_extraction::collector_api {

namespace {

constexpr std::string_view kCollectorPath{"/api/v1/collector"};
constexpr std::string_view kStartPath{"/api/v1/collector/start"};
constexpr std::string_view kStopPath{"/api/v1/collector/stop"};
constexpr std::string_view kAutoModePath{"/api/v1/collector/mode/auto"};
constexpr std::string_view kForcedOnModePath{
    "/api/v1/collector/mode/forced-on"};
constexpr std::string_view kForcedOffModePath{
    "/api/v1/collector/mode/forced-off"};

constexpr int kOk = 200;
constexpr int kAccepted = 202;
constexpr int kNotFound = 404;
constexpr int kMethodNotAllowed = 405;
constexpr int kConflict = 409;

std::string_view mode_name(collector_unit::CollectorMode mode) noexcept {
  using collector_unit::CollectorMode;
  switch (mode) {
  case CollectorMode::automatic:
    return "AUTO";
  case CollectorMode::forced_on:
    return "FORCED_ON";
  case CollectorMode::forced_off:
    return "FORCED_OFF";
  }
  return "unknown";
}

std::string escape_json(std::string_view text) {
  std::string out;
  for (const char c : text) {
    if (c == '"' || c == '\\') {
      out.push_back('\\');
      out.push_back(c);
    } else if (static_cast<unsigned char>(c) < 0x20) {
      out.push_back(' ');
    } else {
      out.push_back(c);
    }
  }
  return out;
}

Response error(int status, std::string_view message) {
  return {status, "{\"error\":\"" + escape_json(message) + "\"}"};
}

} // namespace

std::string_view state_name(collector_unit::CollectorState state) noexcept {
  using collector_unit::CollectorState;
  switch (state) {
  case CollectorState::stopped:
    return "stopped";
  case CollectorState::starting:
    return "starting";
  case CollectorState::running:
    return "running";
  case CollectorState::stopping:
    return "stopping";
  case CollectorState::faulted:
    return "faulted";
  }
  return "unknown";
}

std::string CollectorApi::state_json() const {
  return "{\"state\":\"" + std::string(state_name(control_.state())) +
         "\",\"device_uid\":\"" + escape_json(control_.device_uid()) +
         "\",\"mode\":\"" + std::string(mode_name(control_.mode())) +
         "\",\"demand_active\":" +
         (control_.demand_active() ? "true" : "false") +
         ",\"demand_suppressed\":" +
         (control_.demand_suppressed() ? "true" : "false") + "}";
}

Response CollectorApi::handle(Method method, std::string_view path) const {
  if (path == kCollectorPath) {
    if (method != Method::get) {
      return error(kMethodNotAllowed, "method not allowed");
    }
    return {kOk, state_json()};
  }

  using collector_unit::CollectorMode;
  CollectorMode requested_mode;
  const bool is_mode_request = path == kAutoModePath ||
                               path == kForcedOnModePath ||
                               path == kForcedOffModePath;
  if (is_mode_request) {
    if (method != Method::post) {
      return error(kMethodNotAllowed, "method not allowed");
    }
    if (path == kAutoModePath) {
      requested_mode = CollectorMode::automatic;
    } else if (path == kForcedOnModePath) {
      requested_mode = CollectorMode::forced_on;
    } else {
      requested_mode = CollectorMode::forced_off;
    }
    if (!control_.request_mode(requested_mode)) {
      return error(kConflict,
                   "mode request not permitted or output not delivered");
    }
    return {kAccepted, state_json()};
  }

  const bool is_start = path == kStartPath;
  if (is_start || path == kStopPath) {
    if (method != Method::post) {
      return error(kMethodNotAllowed, "method not allowed");
    }
    const bool accepted =
        is_start ? control_.request_start() : control_.request_stop();
    if (!accepted) {
      return error(kConflict, "request not permitted in current state");
    }
    // Commands only request a change; the body reports the state observed now.
    return {kAccepted, state_json()};
  }

  return error(kNotFound, "not found");
}

} // namespace open_extraction::collector_api
