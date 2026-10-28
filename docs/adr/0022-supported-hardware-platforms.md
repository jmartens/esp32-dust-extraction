# ADR-0022: Supported Hardware Platforms

**Status:** Proposed

## Context

The architecture already treats a Physical Device as platform neutral
([ADR-0014](0014-physical-devices-units-and-capabilities.md)) and the
[Device Information](../specification/08-device-information.md) specification
lists ESP32 and ESP8266 as examples. Business logic is transport independent
([ADR-0011](0011-transport-agnostic-business-logic.md)) and testable without
hardware.

The only buildable firmware today targets ESP32 with ESP-IDF. ESP-IDF does not
support the ESP8266, which is a cheap and common option for satellite devices.

## Decision

- ESP32 is the primary platform and the only platform for the Controller.
- ESP8266 MAY be supported for satellite devices (Machine, Gate and sensor
  roles) that do not need the Controller services.
- The shared core (events, commands, dispatchers, units, state machines,
  routing, message codec and signing) SHALL be standard C++ and SHALL NOT include
  platform headers such as `esp_*`, `driver/*`, `freertos/*`, `nvs*` or `mbedtls/*`.
- Platform specific behaviour SHALL sit behind interfaces implemented once per
  platform: transport (ESP-NOW), random source, persistent storage, GPIO/relay,
  MAC address, time and logging.
- The wire protocol SHALL be identical on all platforms. Signing SHALL use a
  portable implementation so that signatures verify across platforms.
- Satellites SHALL NOT depend on JSON or on ESP-IDF components. Satellite
  messages SHALL use the compact binary protocol and SHALL fit the ESP-NOW
  payload limit of 250 bytes, including header and signature.
- Each platform MAY use its own build system, provided it compiles the same
  shared sources. Host builds of the shared core remain the primary test path.

## Consequences

Advantages:

- cheaper and reusable satellite hardware
- enforces the existing transport and hardware independence rules
- the shared core stays testable on a host

Disadvantages:

- more build configurations to maintain and test in CI
- ESP8266 has less memory, no hardware crypto and no NVS, so storage and
  crypto need platform backends
- ESP-NOW channel handling differs between platforms and must be validated on
  real hardware

## Status of Support

ESP8266 support is planned, not implemented. Documentation and releases SHALL
NOT claim ESP8266 support until a satellite firmware builds and passes hardware
verification.
