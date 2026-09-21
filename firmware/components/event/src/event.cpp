#include "open_extraction/event/event.hpp"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <utility>

namespace open_extraction::event {
namespace {

constexpr std::size_t kUuidLength = 36;
constexpr std::size_t kDeviceUidHexLength = 12;

bool is_hexadecimal(char character) {
    return std::isxdigit(static_cast<unsigned char>(character)) != 0;
}

bool is_uuid(std::string_view value) {
    if (value.length() != kUuidLength) {
        return false;
    }

    for (std::size_t index = 0; index < value.length(); ++index) {
        if (index == 8 || index == 13 || index == 18 || index == 23) {
            if (value[index] != '-') {
                return false;
            }
            continue;
        }

        if (!is_hexadecimal(value[index])) {
            return false;
        }
    }

    return true;
}

bool is_device_type_character(char character) {
    return (character >= 'A' && character <= 'Z') ||
           (character >= '0' && character <= '9') || character == '_';
}

bool is_device_uid(std::string_view value) {
    const auto separator = value.rfind('-');
    if (separator == std::string_view::npos || separator == 0 ||
        value.length() != separator + 1 + kDeviceUidHexLength) {
        return false;
    }

    for (std::size_t index = 0; index < separator; ++index) {
        if (!is_device_type_character(value[index])) {
            return false;
        }
    }

    for (std::size_t index = separator + 1; index < value.length(); ++index) {
        if (!is_hexadecimal(value[index])) {
            return false;
        }
    }

    return true;
}

}  // namespace

EventId EventId::from_bytes(const std::array<std::uint8_t, 16>& bytes) {
    std::ostringstream formatted;
    formatted << std::hex << std::setfill('0');

    for (std::size_t index = 0; index < bytes.size(); ++index) {
        formatted << std::setw(2) << static_cast<unsigned int>(bytes[index]);
        if (index == 3 || index == 5 || index == 7 || index == 9) {
            formatted << '-';
        }
    }

    return EventId(formatted.str());
}

std::optional<EventId> EventId::from_string(std::string value) {
    if (!is_uuid(value)) {
        return std::nullopt;
    }

    return EventId(std::move(value));
}

EventId::EventId(std::string value) : value_(std::move(value)) {}

std::string_view EventId::value() const noexcept {
    return value_;
}

std::optional<EventType> EventType::from_string(std::string value) {
    if (value.empty()) {
        return std::nullopt;
    }

    return EventType(std::move(value));
}

EventType::EventType(std::string value) : value_(std::move(value)) {}

std::string_view EventType::value() const noexcept {
    return value_;
}

std::optional<DeviceUid> DeviceUid::from_string(std::string value) {
    if (!is_device_uid(value)) {
        return std::nullopt;
    }

    return DeviceUid(std::move(value));
}

DeviceUid::DeviceUid(std::string value) : value_(std::move(value)) {}

std::string_view DeviceUid::value() const noexcept {
    return value_;
}

Event::Event(EventMetadata metadata) : metadata_(std::move(metadata)) {}

std::string_view Event::event_id() const noexcept {
    return metadata_.id.value();
}

std::string_view Event::event_type() const noexcept {
    return metadata_.type.value();
}

Timestamp Event::occurred_at() const noexcept {
    return metadata_.occurred_at;
}

std::string_view Event::originating_device() const noexcept {
    return metadata_.originating_device.value();
}

}  // namespace open_extraction::event
