# Architecture Decision Records

Architecture Decision Records (ADRs) capture accepted architectural decisions
and their rationale. New architectural changes should add or update an ADR
before implementation, as described in [ADR-0001](0001-architecture-first.md).

## Index

| Record | Decision | Status |
| --- | --- | --- |
| [ADR-0001](0001-architecture-first.md) | Architecture First Development | Accepted |
| [ADR-0002](0002-controller-authority.md) | Controller is the Sole Authority | Accepted |
| [ADR-0003](0003-immutable-device-identity.md) | Immutable Device Identity | Accepted |
| [ADR-0004](0004-routing-configuration-is-controller-owned.md) | Routing Configuration is Controller-Owned | Accepted |
| [ADR-0005](0005-use-rfc-2119-normative-language.md) | RFC 2119 Normative Language | Draft or empty |
| [ADR-0006](0006-capabilities-over-device-types.md) | Capabilities over Device Implementations | Accepted |
| [ADR-0007](0007-local-hardware-protection.md) | Local Hardware Protection | Accepted |
| [ADR-0008](0008-event-driven-controller-core.md) | Event-Driven Controller Core | Accepted |
| [ADR-0009](0009-separate-event-bus-and-command-bus.md) | Separate Event Bus and Command Bus | Accepted |
| [ADR-0010](0010-event-based-service-communication.md) | Event-Based Service Communication | Accepted |
| [ADR-0011](0011-transport-agnostic-business-logic.md) | Transport-Agnostic Business Logic | Accepted |
| [ADR-0012a](0012-controller-as-a-reactive-state-machine.md) | Controller as a Reactive State Machine | Draft or incomplete |
| [ADR-0012b](0012-controller-owned-resource-namespace.md) | Controller-Owned Resource Namespace | Accepted |
| [ADR-0013](0013-domain-events-and-infrastructure-eventss.md) | Domain Events and Infrastructure Events | Accepted |
| [ADR-0014](0014-physical-devices-units-and-capabilities.md) | Physical Devices, Units, and Capabilities | Accepted |
| [ADR-0016](0016-service-lifecycle-and-self-Test-Semantics..md) | System Health, Initialization, and Self-Test | Accepted |
| [ADR-0017](0017-health-hierarchy-and-health-states.md) | Health Hierarchy and Health States | Accepted |
| [ADR-0018](0018-versioning-strategy.md) | Versioning Strategy | Draft or empty |
| [ADR-0019](0019-time-abstraction.md) | Time Abstraction | Draft or empty |

## Status and Maintenance

The filenames and identifiers above reflect the current repository. A small
number of records are placeholders or have incomplete metadata; they remain
listed so the index does not hide committed architectural material. Those
records should be completed or explicitly retired in a follow-up ADR task.

When an ADR changes an authoritative specification, update the relevant
specification and add cross-references in both documents. ADR links should use
repository-relative paths so they remain valid in local checkouts and on
GitHub.
