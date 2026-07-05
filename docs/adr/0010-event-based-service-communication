# ADR-0010: Event-Based Service Communication

**Status:** Accepted

## Context

As the Controller evolves, it is responsible for multiple independent domains, including:

- Device Registry
- Routing
- Collector Control
- Configuration
- Security
- Discovery
- Diagnostics

Implementing these domains as tightly coupled modules that directly invoke one another's behaviour would increase complexity, reduce maintainability and make testing difficult.

The project has already adopted an event-driven Controller core (ADR-0008) and separate Event and Command buses (ADR-0009). A communication model between services is therefore required.

## Decision

The Controller SHALL be composed of independent services.

Each service SHALL own its internal state.

Services SHALL communicate behavioural changes exclusively by publishing Events.

Services MAY subscribe to one or more Event types.

Services MAY publish Events.

Services MAY emit Commands.

Services MAY perform synchronous read-only queries against other services.

Services SHALL NOT directly invoke behavioural operations on other services.

## Rationale

Read-only queries do not alter system behaviour and therefore do not violate separation of concerns.

Behavioural operations, however, affect overall system state and SHALL always be represented as Events.

Publishing Events allows multiple interested services to observe and react to the same behavioural change without introducing direct coupling.

This architecture promotes loose coupling, high cohesion and deterministic behaviour.

## Example

Correct:

```
MachineDemandStarted (Event)
            │
            ▼
     Routing Service
            │
            ▼
     RouteActivated (Event)
            │
            ▼
    Collector Service
            │
            ▼
   StartCollector (Command)
```

Incorrect:

```
RoutingService::startCollector();
```

In the incorrect example, the Routing Service directly invokes behaviour in another service, creating unnecessary coupling.

## Consequences

Advantages:

- Loose coupling between services.
- High cohesion.
- Independent unit testing.
- Easy addition of new services.
- Multiple services can observe the same Event.
- Behaviour is fully observable through the Event stream.

Disadvantages:

- Increased number of Events.
- Slightly more complex implementation.

These disadvantages are acceptable because they significantly improve long-term maintainability and extensibility.