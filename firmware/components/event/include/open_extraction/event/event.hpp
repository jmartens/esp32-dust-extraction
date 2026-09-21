#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace open_extraction::event {

using Timestamp = std::chrono::sys_time<std::chrono::milliseconds>;

class EventId {
public:
    [[nodiscard]] static EventId from_bytes(
        const std::array<std::uint8_t, 16>& bytes);
    [[nodiscard]] static std::optional<EventId> from_string(std::string value);

    [[nodiscard]] std::string_view value() const noexcept;

private:
    explicit EventId(std::string value);

    std::string value_;
};

class EventIdGenerator {
public:
    virtual ~EventIdGenerator() = default;

    [[nodiscard]] virtual EventId generate() = 0;
};

class EventType {
public:
    [[nodiscard]] static std::optional<EventType> from_string(std::string value);

    [[nodiscard]] std::string_view value() const noexcept;

private:
    explicit EventType(std::string value);

    std::string value_;
};

class DeviceUid {
public:
    [[nodiscard]] static std::optional<DeviceUid> from_string(std::string value);

    [[nodiscard]] std::string_view value() const noexcept;

private:
    explicit DeviceUid(std::string value);

    std::string value_;
};

struct EventMetadata {
    EventId id;
    EventType type;
    Timestamp occurred_at;
    DeviceUid originating_device;
};

class Event {
public:
    Event(const Event&) = default;
    Event(Event&&) noexcept = default;
    Event& operator=(const Event&) = default;
    Event& operator=(Event&&) noexcept = default;
    virtual ~Event() = default;

    [[nodiscard]] std::string_view event_id() const noexcept;
    [[nodiscard]] std::string_view event_type() const noexcept;
    [[nodiscard]] Timestamp occurred_at() const noexcept;
    [[nodiscard]] std::string_view originating_device() const noexcept;

protected:
    explicit Event(EventMetadata metadata);

private:
    EventMetadata metadata_;
};

}  // namespace open_extraction::event
