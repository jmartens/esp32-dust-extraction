#include "collector_unit/collector_unit.hpp"
#include "open_extraction/collector_unit/relay_backed_collector_unit.hpp"
#include "relay/relay_driver.hpp"
#include "unity.h"

namespace {

using open_extraction::collector_unit::CollectorCapability;
using open_extraction::collector_unit::CollectorDiagnostics;
using open_extraction::collector_unit::CollectorState;
using open_extraction::collector_unit::RelayBackedCollectorUnit;
using open_extraction::relay::RelayDriver;
using open_extraction::relay::RelayState;

class FakeRelay final : public RelayDriver {
public:
  void on() override {
    state_ = RelayState::on;
    ++on_calls;
  }

  void off() override {
    state_ = RelayState::off;
    ++off_calls;
  }

  [[nodiscard]] RelayState state() const override { return state_; }

  int on_calls{};
  int off_calls{};
  RelayState state_{RelayState::off};
};

} // namespace

TEST_CASE("collector unit exposes its capabilities", "[collector_unit]") {
  FakeRelay relay;
  const RelayBackedCollectorUnit collector(relay);
  const auto capabilities = collector.capabilities();

  TEST_ASSERT_EQUAL_UINT8(1, capabilities.size());
  TEST_ASSERT_EQUAL(static_cast<int>(CollectorCapability::start_stop),
                    static_cast<int>(capabilities[0]));
}

TEST_CASE("collector unit initializes the relay to off", "[collector_unit]") {
  FakeRelay relay;
  relay.on();
  RelayBackedCollectorUnit collector(relay);

  TEST_ASSERT_EQUAL(static_cast<int>(RelayState::off),
                    static_cast<int>(relay.state()));
  TEST_ASSERT_EQUAL(static_cast<int>(CollectorState::stopped),
                    static_cast<int>(collector.state()));
  TEST_ASSERT_EQUAL_INT(1, relay.on_calls);
  TEST_ASSERT_EQUAL_INT(1, relay.off_calls);
}

TEST_CASE("start and stop drive the relay and report commanded state",
          "[collector_unit]") {
  FakeRelay relay;
  RelayBackedCollectorUnit collector(relay);

  TEST_ASSERT_TRUE(collector.start());
  TEST_ASSERT_EQUAL(static_cast<int>(RelayState::on),
                    static_cast<int>(relay.state()));
  TEST_ASSERT_EQUAL(static_cast<int>(CollectorState::running),
                    static_cast<int>(collector.state()));
  TEST_ASSERT_TRUE(collector.stop());
  TEST_ASSERT_EQUAL(static_cast<int>(RelayState::off),
                    static_cast<int>(relay.state()));
  TEST_ASSERT_EQUAL(static_cast<int>(CollectorState::stopped),
                    static_cast<int>(collector.state()));

  const auto diagnostics = collector.diagnostics();
  TEST_ASSERT_EQUAL_UINT32(1, diagnostics.successful_starts);
  TEST_ASSERT_EQUAL_UINT32(1, diagnostics.successful_stops);
  TEST_ASSERT_EQUAL_UINT32(0, diagnostics.fault_count);
}

TEST_CASE("rejected operations do not change the relay output",
          "[collector_unit]") {
  FakeRelay relay;
  RelayBackedCollectorUnit collector(relay);
  const auto initial_off_calls = relay.off_calls;

  TEST_ASSERT_FALSE(collector.stop());
  TEST_ASSERT_EQUAL(static_cast<int>(RelayState::off),
                    static_cast<int>(relay.state()));
  TEST_ASSERT_EQUAL_INT(0, relay.on_calls);
  TEST_ASSERT_EQUAL_INT(initial_off_calls, relay.off_calls);

  TEST_ASSERT_TRUE(collector.start());
  TEST_ASSERT_FALSE(collector.start());
  TEST_ASSERT_EQUAL(static_cast<int>(RelayState::on),
                    static_cast<int>(relay.state()));
  TEST_ASSERT_EQUAL_INT(1, relay.on_calls);

  TEST_ASSERT_TRUE(collector.stop());
  TEST_ASSERT_FALSE(collector.stop());
  TEST_ASSERT_EQUAL(static_cast<int>(RelayState::off),
                    static_cast<int>(relay.state()));
  TEST_ASSERT_EQUAL_INT(2, relay.off_calls);
  TEST_ASSERT_EQUAL_INT(1, relay.on_calls);
}
