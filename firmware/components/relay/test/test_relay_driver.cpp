#include <cstdint>

#include "relay/relay_driver.hpp"
#include "unity.h"

namespace {

class TestRelayDriver final : public open_extraction::relay::RelayDriver {
public:
  void on() override {
    current_state_ = open_extraction::relay::RelayState::on;
  }

  void off() override {
    current_state_ = open_extraction::relay::RelayState::off;
  }

  [[nodiscard]] open_extraction::relay::RelayState state() const override {
    return current_state_;
  }

private:
  open_extraction::relay::RelayState current_state_ =
      open_extraction::relay::RelayState::off;
};

} // namespace

TEST_CASE("relay driver turns on", "[relay]") {
  TestRelayDriver relay;

  relay.on();

  TEST_ASSERT_TRUE(relay.is_on());
  TEST_ASSERT_FALSE(relay.is_off());
  TEST_ASSERT_EQUAL_UINT8(
      static_cast<std::uint8_t>(open_extraction::relay::RelayState::on),
      static_cast<std::uint8_t>(relay.state()));
}

TEST_CASE("relay driver turns off", "[relay]") {
  TestRelayDriver relay;

  relay.on();
  relay.off();

  TEST_ASSERT_FALSE(relay.is_on());
  TEST_ASSERT_TRUE(relay.is_off());
  TEST_ASSERT_EQUAL_UINT8(
      static_cast<std::uint8_t>(open_extraction::relay::RelayState::off),
      static_cast<std::uint8_t>(relay.state()));
}
