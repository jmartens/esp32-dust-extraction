# ADR-0012: Controller-Owned Resource Namespace

**Status:** Accepted

## Context

The OpenExtraction Controller manages configuration, diagnostics, statistics and operational actions for all devices within the system.

Early protocol designs represented configurable properties as flat Object IDs and operation-specific messages.

As the architecture evolved, it became apparent that all interactions with devices can be modelled as operations on resources owned by the Controller.

A hierarchical resource namespace provides a consistent abstraction that can be exposed through multiple communication transports, including:

- ESP-NOW
- Embedded Web API
- Future MQTT integration
- Future command-line tools
- Future Home Assistant integration

The underlying transport and wire encoding should not influence the logical organisation of system resources.

## Decision

The Controller SHALL own the global resource namespace.

Resources SHALL be organised hierarchically.

Every resource SHALL have exactly one canonical location within the namespace.

Resources SHALL be uniquely identifiable.

Nodes SHALL NOT define or extend the resource namespace.

Nodes SHALL expose only those resources defined by the Controller.

Operations SHALL target resources rather than device-specific message types.

The resource namespace SHALL remain independent of the underlying communication transport.

The wire protocol MAY encode resources using compact binary identifiers.

The logical organisation of resources SHALL remain hierarchical regardless of wire encoding.

## Example Resource Hierarchy

```
/controller
/controller/config
/controller/status
/controller/statistics
/controller/security

/machines/{uid}
/machines/{uid}/config
/machines/{uid}/status
/machines/{uid}/statistics
/machines/{uid}/actions

/gates/{uid}
/gates/{uid}/config
/gates/{uid}/status
/gates/{uid}/statistics
/gates/{uid}/actions

/devices/{uid}
/devices/{uid}/status
```

The hierarchy shown above is illustrative and may be extended by future specifications.

## Rationale

A hierarchical resource namespace provides:

- a uniform programming model
- transport independence
- consistent documentation
- natural mapping to REST-style APIs
- straightforward future MQTT mapping
- simplified tooling
- improved extensibility

The Controller remains the sole authority defining available resources and their semantics.

## Consequences

### Advantages

- Single global namespace.
- Consistent interaction model.
- Cleaner protocol specification.
- Simplified Web API implementation.
- Simplified client implementations.
- Reduced protocol complexity.
- Easier future expansion.

### Disadvantages

- Initial design requires careful namespace planning.
- Future specifications must preserve namespace consistency.

These disadvantages are considered acceptable because they significantly improve long-term maintainability and extensibility.

# TODO
Merge resource collection and Resource Instance into ADR

/machines              ← Resource Collection
/machines/{uid}        ← Resource Instance
/machines/{uid}/status ← Child Resource
/machines/{uid}/actions/start-calibration ← Action Resource