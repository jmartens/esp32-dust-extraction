#pragma once

#include <cstdint>

namespace open_extraction::relay {

enum class RelayState : std::uint8_t {
  off,
  on,
};

class RelayDriver {
public:
  virtual ~RelayDriver() = default;

  virtual void on() = 0;
  virtual void off() = 0;

  [[nodiscard]] virtual RelayState state() const = 0;

  [[nodiscard]] bool is_on() const { return state() == RelayState::on; }

  [[nodiscard]] bool is_off() const { return state() == RelayState::off; }
};

} // namespace open_extraction::relay
