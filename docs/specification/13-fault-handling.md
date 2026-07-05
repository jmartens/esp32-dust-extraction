# Fault Handling

## Purpose

This document defines the fault handling model of the OpenExtraction Controller.

The Fault Handling Model specifies:

- what constitutes a fault;
- how faults are represented;
- how faults are handled by the Controller;
- ownership of fault information.

Implementation-specific errors are intentionally outside the scope of this specification.

---

# Principles

The Fault Handling Model is founded upon the following principles.

- Faults are domain concepts.
- Faults SHALL be represented as Domain Events.
- Fault handling SHALL be deterministic.
- Fault ownership SHALL be explicit.
- Fault handling SHALL remain transport independent.
- Fault handling SHALL NOT require knowledge of hardware implementations.

---

# Faults

A Fault represents an operational problem within the OpenExtraction domain.

Examples include:

- Gate fault
- Collector fault
- Machine fault
- Route unavailable
- Calibration failure

Faults SHALL represent business-significant events.

---

# Errors

Errors represent implementation or infrastructure problems.

Examples include:

- malformed packets;
- storage failures;
- transport failures;
- internal software errors;
- protocol violations.

Errors SHALL NOT be represented as Domain Events unless explicitly elevated to a Fault by the owning component.

---

# Fault Representation

Faults SHALL be represented by Domain Events.

Examples include:

- GateFault
- CollectorFault
- MachineFault
- RouteUnavailable
- CalibrationFailed

Faults SHALL be immutable.

---

# Fault Ownership

Every Fault SHALL have exactly one owning Service.

Examples:

| Fault | Owner |
|------|------|
| GateFault | Gate Service |
| RouteUnavailable | Routing Service |
| CollectorFault | Collector Service |
| CalibrationFailed | Configuration Service |

---

# Fault Lifecycle

Faults SHALL follow the following lifecycle.

```text
Fault Occurs

↓

Fault Domain Event

↓

Event Dispatcher

↓

Fault Handling

↓

State Update

↓

Diagnostics Updated

↓

Fault Resolved
```

---

# Fault Detection

Faults MAY originate from:

- Physical Devices;
- Controller Services;
- Configuration validation;
- Controller runtime behaviour.

The owning Service SHALL determine whether a condition constitutes a Fault.

---

# Fault State

Services MAY maintain fault state where required.

Examples include:

- Gate fault active;
- Collector unavailable;
- Route unavailable.

Fault state SHALL remain owned by the responsible Service.

---

# Fault Propagation

A Fault MAY affect multiple Services.

Example:

```text
GateFault

↓

Routing Service

↓

RouteUnavailable

↓

Collector Service

↓

Collector Start Prevented
```

Fault propagation SHALL occur exclusively through Domain Events.

---

# Recovery

Fault recovery SHALL be deterministic.

Examples include:

- automatic retry;
- manual intervention;
- timeout expiration;
- configuration changes.

The recovery mechanism SHALL be determined by the owning Service.

---

# Route Faults

The Routing Service SHALL determine the impact of faults upon active Routes.

Examples include:

- unavailable Gates;
- unavailable Units;
- invalid Route definitions.

The Routing Service SHALL determine whether a Route may remain active.

---

# Device Faults

Physical Devices MAY report faults.

Examples include:

- overcurrent;
- calibration failures;
- actuator failures.

Physical Device faults SHALL be translated into Domain Events before entering the Controller.

---

# Manual Operation

Manual operation SHALL NOT bypass fault handling.

Controller Services SHALL determine whether manual operations are permitted during fault conditions.

---

# Fault Persistence

Fault information MAY be persisted.

Examples include:

- active faults;
- fault history;
- diagnostic information.

Persistence policies are defined elsewhere.

---

# Diagnostics

The Diagnostics Service SHALL expose diagnostic information relating to faults.

Examples include:

- fault history;
- fault counters;
- fault timestamps.

The Diagnostics Service SHALL NOT own fault state.

---

# Design Principles

The Fault Handling Model SHALL satisfy the following principles.

- Explicit ownership.
- Deterministic fault handling.
- Faults are Domain Events.
- Errors are implementation concerns.
- Fault handling is transport independent.
- Fault propagation occurs through Domain Events.