# ADR-0008: Event-Driven Controller Core

**Status:** Accepted

## Context

The OpenExtraction Controller receives input from multiple independent sources, including:

- ESP-NOW messages
- Embedded web interface
- Physical buttons and switches
- Timers
- Internal watchdogs
- Future external integrations

Allowing these inputs to directly manipulate Controller state would tightly couple transport mechanisms, hardware interfaces and business logic.

Such coupling would make the Controller difficult to test, extend and maintain.

## Decision

The Controller SHALL implement an internal event-driven architecture.

All external inputs SHALL be translated into events before they are processed by the Controller.

Business logic SHALL operate exclusively on events.

Business logic SHALL NOT directly depend on:

- ESP-NOW
- HTTP
- GPIO
- timers
- storage implementation

Controller subsystems SHALL communicate by publishing and consuming events.

The Controller SHALL be the sole owner of system state.

## Event Sources

Examples of event sources include:

- Device messages
- Hardware inputs
- Timer expiration
- Configuration changes
- Device discovery
- User interaction

Future event sources MAY be introduced without modifying existing business logic.

## Event Consumers

Examples of event consumers include:

- Routing Engine
- Collector State Machine
- Device Registry
- Configuration Manager
- Diagnostics
- Event Logger

## Consequences

Advantages:

- Clear separation between transport and business logic.
- Hardware-independent Controller logic.
- Easier unit testing.
- Easier simulation on desktop platforms.
- Simplified addition of future communication transports.
- Reduced coupling between subsystems.
- Improved maintainability.

Disadvantages:

- Slightly increased architectural complexity.
- Introduction of an event dispatching mechanism.

These trade-offs are considered acceptable because they significantly improve long-term maintainability and extensibility.

## Example

The following sequence illustrates the intended processing model:

```
ESP-NOW Packet
        │
        ▼
Message Decoder
        │
        ▼
MachineDemandChanged Event
        │
        ▼
Event Dispatcher
        │
        ▼
Routing Engine
        │
        ▼
Collector State Machine
        │
        ▼
Command
        │
        ▼
ESP-NOW Message
```

At no point shall transport-specific code directly manipulate Controller state.