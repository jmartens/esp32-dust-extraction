#pragma once

#include <string>
#include <string_view>

#include "collector_unit/collector_unit.hpp"

namespace open_extraction::collector_api {

enum class Method { get, post };

struct Response {
  int status;
  std::string body;
};

// Narrow port the API depends on. Implementations issue commands and observe
// state; the API itself never changes collector state (ADR-0009, ADR-0011).
class ICollectorControl {
public:
  virtual ~ICollectorControl() = default;

  [[nodiscard]] virtual collector_unit::CollectorState state() const = 0;
  [[nodiscard]] virtual std::string device_uid() const = 0;
  // Return true when the request was accepted for processing.
  [[nodiscard]] virtual bool request_start() = 0;
  [[nodiscard]] virtual bool request_stop() = 0;
};

[[nodiscard]] std::string_view
state_name(collector_unit::CollectorState state) noexcept;

// Transport-agnostic handler for the /api/v1/collector routes.
class CollectorApi {
public:
  explicit CollectorApi(ICollectorControl &control) noexcept
      : control_(control) {}

  [[nodiscard]] Response handle(Method method, std::string_view path) const;

private:
  [[nodiscard]] std::string state_json() const;

  ICollectorControl &control_;
};

} // namespace open_extraction::collector_api
