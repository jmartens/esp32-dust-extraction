# ADR-0021: UUID Version 4 Event Identifiers

**Status:** Accepted

## Context

Every event carries an `EventId` (see the [Event Model](../specification/03-event-model.md)),
which is used to correlate, log and trace events. The firmware represents it as
a canonical 36-character UUID string and generates UUID version 4 values on the
ESP32 using its hardware random number generator.

Event IDs are generated independently on each device and on the controller. There
is no central allocator, and requiring one would add coupling and a failure
mode to the event-driven design in
[ADR-0008](0008-event-driven-controller-core.md).

## Decision

UUID version 4 (122 random bits) is accepted as sufficiently unique for event
identifiers. The system shall not implement collision detection, central
allocation or coordination for event IDs.

Uniqueness is probabilistic, not guaranteed. With 2^122 possible values, the
probability of any collision among n IDs is approximately n² / 2^123. Even at
10^9 events the probability is below 10^-19, many orders of magnitude below the
rate of hardware failure and other fault sources in this system.

## Scope

- Applies to event identifiers only (`EventId`).
- Event IDs are correlation and tracing identifiers. They are not security
  tokens, are not authentication or authorization material, and shall not be
  treated as secrets.
- Event IDs do not replace device identity ([ADR-0003](0003-immutable-device-identity.md)).
- Behaviour shall not depend on event ID uniqueness for safety. Hardware
  protection ([ADR-0007](0007-local-hardware-protection.md)) remains independent.
- Event IDs shall be generated from a cryptographically suitable or hardware
  entropy source, not from a predictable counter or clock.

## Consequences

Advantages:

- no coordination, allocation or persistent counter required
- IDs can be created offline on any node
- standard, widely supported format

Disadvantages:

- a collision is theoretically possible; consumers shall tolerate a duplicate
  ID by treating it as a diagnostic anomaly, not as a correctness failure
- a low-quality entropy source would weaken the assumption and shall be treated
  as a defect

Revisit this decision if events become security-relevant, require ordering, or
IDs must be deduplicated for exactly-once processing.
