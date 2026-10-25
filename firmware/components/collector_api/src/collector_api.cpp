#include "open_extraction/collector_api/collector_api.hpp"

namespace open_extraction::collector_api {

namespace {

constexpr std::string_view kCollectorPath{"/api/v1/collector"};
constexpr std::string_view kStartPath{"/api/v1/collector/start"};
constexpr std::string_view kStopPath{"/api/v1/collector/stop"};

constexpr int kOk = 200;
constexpr int kAccepted = 202;
constexpr int kNotFound = 404;
constexpr int kMethodNotAllowed = 405;
constexpr int kConflict = 409;

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
         "\",\"device_uid\":\"" + escape_json(control_.device_uid()) + "\"}";
}

Response CollectorApi::handle(Method method, std::string_view path) const {
  if (path == kCollectorPath) {
    if (method != Method::get) {
      return error(kMethodNotAllowed, "method not allowed");
    }
    return {kOk, state_json()};
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
