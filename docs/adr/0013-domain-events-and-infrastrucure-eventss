# ADR-0013: Domain Events and Infrastructure Events

**Status:** Accepted

## Context

The OpenExtraction Controller is implemented as an event-driven system.

During protocol design it became apparent that not every internal state change represents a meaningful business event.

Examples include:

- heartbeat processing
- sequence number tracking
- cache updates
- replay detection
- device online status
- device offline status
- transport retries

These state changes are implementation details of individual services.

Publishing all internal state changes onto the Event Bus would unnecessarily couple unrelated services and expose infrastructure concerns to business logic.

## Decision

Only business-significant facts SHALL be published as Events.

Infrastructure state SHALL remain private to the owning service.

Services SHALL own their internal implementation state.

Services MAY expose internal state through synchronous read-only queries.

Infrastructure state SHALL NOT be published onto the Event Bus unless it has explicit business significance.

Business logic SHALL depend only upon Domain Events.

## Domain Events

Examples include:

- MachineDemandStarted
- MachineDemandStopped
- GateOpened
- GateClosed
- GateFault
- CollectorStarted
- CollectorStopped
- AllRequiredGatesOpen
- RequiredGateUnavailable

These Events represent facts that influence business behaviour and may be consumed by multiple services.

## Infrastructure Events

Examples include:

- Heartbeat received
- Heartbeat timeout
- Sequence number updated
- Peer connected
- Peer disconnected
- Packet retransmitted
- Signature verified
- Device registry updated

These are implementation details.

Infrastructure Events SHALL remain internal to the service responsible for them.

## Example

Heartbeat timeout:

```
Heartbeat Timeout

↓

Device Registry

↓

Online = false
```

No Event is published.

Business consequence:

```
Heartbeat Timeout

↓

Device Registry

↓

Required gate unavailable

↓

RequiredGateUnavailable (Event)
```

Only the business-significant fact is published.

## Rationale

Separating Domain Events from Infrastructure Events:

- reduces coupling
- keeps the Event Bus focused on business behaviour
- simplifies testing
- improves maintainability
- prevents leakage of implementation details

Services remain free to evolve internally without affecting other services provided that published Domain Events remain unchanged.

## Consequences

### Advantages

- Smaller Event Bus.
- Reduced coupling.
- Cleaner service boundaries.
- Better encapsulation.
- Improved scalability.
- Simpler reasoning about business behaviour.

### Disadvantages

- Services require additional internal state management.
- Developers must distinguish carefully between business and infrastructure concerns.

These disadvantages are considered acceptable because they preserve clear separation between infrastructure and business logic while improving long-term maintainability.