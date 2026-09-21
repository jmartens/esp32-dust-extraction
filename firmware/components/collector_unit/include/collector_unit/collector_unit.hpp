#pragma once

#include <cstdint>
#include <span>

namespace open_extraction::collector_unit {

enum class CollectorState : std::uint8_t {
    stopped,
    starting,
    running,
    stopping,
    faulted,
};

enum class CollectorCapability : std::uint8_t {
    start_stop,
    fault_reporting,
};

struct CollectorDiagnostics {
    std::uint32_t successful_starts{};
    std::uint32_t successful_stops{};
    std::uint32_t fault_count{};
};

class ICollectorUnit {
public:
    virtual ~ICollectorUnit() = default;

    virtual bool start() = 0;
    virtual bool stop() = 0;

    [[nodiscard]] virtual CollectorState state() const = 0;
    [[nodiscard]] virtual CollectorDiagnostics diagnostics() const = 0;
    [[nodiscard]] virtual std::span<const CollectorCapability> capabilities() const = 0;
};

}  // namespace open_extraction::collector_unit
