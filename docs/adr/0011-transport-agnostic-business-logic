# ADR-0011: Transport-Agnostic Business Logic

**Status:** Accepted

## Context

The Controller exchanges information with external devices and users through communication transports.

Current transports include:

- ESP-NOW
- Embedded Web UI

Future transports may include:

- MQTT
- Home Assistant
- Ethernet
- CAN bus
- WebSockets
- Serial
- REST API

Embedding transport-specific behaviour within business logic would tightly couple the Controller to specific communication technologies.

Such coupling would make testing, simulation and future expansion unnecessarily difficult.

## Decision

Business logic SHALL be completely independent of communication transports.

Business logic SHALL consume Events.

Business logic SHALL emit Commands.

Transport adapter SHALL translate external protocol messages into Events.

Transport adapter SHALL translate Commands into transport-specific protocol messages.

Business logic SHALL NOT directly invoke transport APIs.

Business logic SHALL NOT depend upon transport-specific data structures.

Business logic SHALL compile independently of ESP-IDF, Arduino framework and networking libraries.

## Responsibilities

Transport adapter is responsible for:

- packet encoding
- packet decoding
- authentication
- replay protection
- retransmission
- message acknowledgement
- transport addressing
- connection management

Business logic is responsible for:

- routing
- collector scheduling
- gate coordination
- configuration
- diagnostics
- state machines
- security policy

## Rationale

Separating transport from business logic allows:

- native desktop unit testing
- deterministic simulation
- easier maintenance
- future protocol additions
- improved code reuse

The Routing Service, for example, shall operate identically regardless of whether a `MachineDemandStarted` Event originated from:

- ESP-NOW
- MQTT
- REST API
- Unit tests
- Simulation

## Example

```
             ESP-NOW Packet
                    │
                    ▼
          ESP-NOW Transport Adapter
                    │
                    ▼
     MachineDemandStarted (Event)
                    │
                    ▼
              Event Dispatcher
                    │
                    ▼
             Routing Service
                    │
                    ▼
          OpenGate (Command)
                    │
                    ▼
            Command Dispatcher
                    │
                    ▼
          ESP-NOW Transport Adapter
                    │
                    ▼
              ESP-NOW Packet
```

The Routing Service has no knowledge of ESP-NOW and operates solely on Events and Commands.

## Consequences

Advantages:

- Transport-independent business logic.
- Native desktop unit testing.
- Hardware-independent simulation.
- Easy addition of future communication transports.
- Reduced coupling.
- Improved maintainability.

Disadvantages:

- Additional transport abstraction layer.
- Slight increase in translation code.

These disadvantages are considered acceptable because they provide significant long-term architectural benefits.