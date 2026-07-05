# ADR-0006: Capabilities over Device Implementations

**Status:** Accepted

## Context

Future Gate Nodes and Machine Nodes may be implemented using different hardware.

For example, a Gate Node may use:

- A relay with timed movement
- A servo
- A DC motor with limit switches
- A linear actuator with position feedback
- A future actuator technology

Designing the Controller around specific hardware implementations would require protocol changes whenever new hardware is introduced.

The Controller should instead reason about what a device can do rather than how it is implemented.

## Decision

Devices SHALL advertise their capabilities during discovery.

The Controller SHALL base its behaviour on advertised capabilities.

The communication protocol SHALL NOT depend on specific hardware implementations.

Examples of capabilities include:

- Position Feedback
- Timed Movement
- Current Measurement
- Auto Calibration
- Manual Override
- Battery Powered

Additional capabilities MAY be introduced in future protocol versions without changing existing behaviour.

## Consequences

Advantages:

- Hardware-independent Controller logic.
- Extensible protocol.
- Simpler integration of future hardware.
- Reduced firmware coupling.
- Improved backwards compatibility.

Disadvantages:

- Slightly more complex discovery process.
- Capability negotiation becomes part of device registration.

These trade-offs are considered acceptable because they significantly improve long-term extensibility.