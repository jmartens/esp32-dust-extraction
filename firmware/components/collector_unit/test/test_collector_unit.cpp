#include <array>
#include <span>

#include "collector_unit/collector_unit.hpp"
#include "unity.h"

namespace {

using open_extraction::collector_unit::CollectorCapability;
using open_extraction::collector_unit::CollectorDiagnostics;
using open_extraction::collector_unit::CollectorState;
using open_extraction::collector_unit::ICollectorUnit;

class FakeCollectorUnit final : public ICollectorUnit {
public:
    bool start() override
    {
        if (state_ != CollectorState::stopped) {
            return false;
        }

        state_ = CollectorState::running;
        ++diagnostics_.successful_starts;
        return true;
    }

    bool stop() override
    {
        if (state_ != CollectorState::running) {
            return false;
        }

        state_ = CollectorState::stopped;
        ++diagnostics_.successful_stops;
        return true;
    }

    CollectorState state() const override
    {
        return state_;
    }

    CollectorDiagnostics diagnostics() const override
    {
        return diagnostics_;
    }

    std::span<const CollectorCapability> capabilities() const override
    {
        return capabilities_;
    }

private:
    std::array<CollectorCapability, 2> capabilities_{
        CollectorCapability::start_stop,
        CollectorCapability::fault_reporting,
    };
    CollectorDiagnostics diagnostics_{};
    CollectorState state_{CollectorState::stopped};
};

}  // namespace

TEST_CASE("collector unit exposes its capabilities", "[collector_unit]")
{
    const FakeCollectorUnit collector;
    const auto capabilities = collector.capabilities();

    TEST_ASSERT_EQUAL_UINT8(2, capabilities.size());
    TEST_ASSERT_EQUAL(
        static_cast<int>(CollectorCapability::start_stop),
        static_cast<int>(capabilities[0]));
    TEST_ASSERT_EQUAL(
        static_cast<int>(CollectorCapability::fault_reporting),
        static_cast<int>(capabilities[1]));
}

TEST_CASE("collector unit reports state and diagnostics", "[collector_unit]")
{
    FakeCollectorUnit collector;

    TEST_ASSERT_EQUAL(
        static_cast<int>(CollectorState::stopped),
        static_cast<int>(collector.state()));
    TEST_ASSERT_TRUE(collector.start());
    TEST_ASSERT_EQUAL(
        static_cast<int>(CollectorState::running),
        static_cast<int>(collector.state()));
    TEST_ASSERT_TRUE(collector.stop());
    TEST_ASSERT_EQUAL(
        static_cast<int>(CollectorState::stopped),
        static_cast<int>(collector.state()));

    const auto diagnostics = collector.diagnostics();
    TEST_ASSERT_EQUAL_UINT32(1, diagnostics.successful_starts);
    TEST_ASSERT_EQUAL_UINT32(1, diagnostics.successful_stops);
    TEST_ASSERT_EQUAL_UINT32(0, diagnostics.fault_count);
}
