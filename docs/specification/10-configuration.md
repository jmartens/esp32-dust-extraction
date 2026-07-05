# Configuration

## Purpose

This document defines the configuration model of the OpenExtraction Controller.

Configuration determines how the Controller behaves and provides user-customisable behaviour.

The Configuration Model is independent of:

- storage mechanisms;
- communication protocols;
- external APIs;
- user interfaces.

Configuration SHALL be owned exclusively by the Configuration Service.

---

# Principles

The Configuration Model is founded upon the following principles.

- Configuration is a domain concept.
- Configuration SHALL be persistent.
- Configuration SHALL be transport independent.
- Configuration SHALL be hardware independent.
- Configuration SHALL be explicitly owned.
- Configuration SHALL be validated before being applied.

---

# Configuration Categories

Configuration SHALL belong to one of the following categories:

- System Configuration
- Route Configuration
- Unit Configuration
- Timing Configuration
- Diagnostic Configuration
- User Metadata

---

# System Configuration

System Configuration defines global Controller behaviour.

Examples include:

- maintenance mode defaults;
- startup behaviour;
- automatic operation defaults.

System Configuration SHALL be owned by the Configuration Service.

---

# Route Configuration

Route Configuration defines the declarative extraction topology of the workshop.

Route definitions are Configuration.

The Routing Service consumes Route definitions but does not own them.

Examples include:

- Route definitions;
- Route enablement;
- Route priorities (if implemented).

Routes SHALL reference Units.

Routes SHALL NOT reference Physical Devices or transport-specific identifiers.

---

# Unit Configuration

Unit Configuration defines behaviour specific to an individual Unit.

Examples include:

Machine Units:

- current thresholds;
- debounce times;
- calibration values.

Gate Units:

- movement timeouts;
- calibration parameters;
- travel limits.

Collector Units:

- startup delays;
- shutdown behaviour;
- safety timeouts.

The exact configuration items SHALL depend upon Unit Type and advertised Capabilities.

---

# Timing Configuration

Timing Configuration defines temporal behaviour.

Examples include:

- run-on timers;
- startup delays;
- gate movement timeouts;
- heartbeat intervals.

Timing Configuration SHALL use implementation-defined time units.

The architectural model intentionally does not prescribe a particular representation.

---

# Diagnostic Configuration

Diagnostic Configuration defines diagnostic behaviour.

Examples include:

- fault retention periods;
- logging verbosity;
- diagnostic counters.

---

# User Metadata

User Metadata provides user-friendly information.

Examples include:

- friendly names;
- descriptions;
- workshop locations.

User Metadata SHALL NOT influence business logic unless explicitly specified elsewhere.

---

# Validation

All configuration SHALL be validated before being applied.

Validation SHALL ensure:

- configuration consistency;
- Unit existence;
- Route correctness;
- value constraints.

Invalid configuration SHALL NOT be applied.

---

# Configuration Lifecycle

Configuration changes SHALL follow the following lifecycle.

```text
Configuration Request

↓

Validation

↓

Configuration Applied

↓

Configuration Persisted

↓

Configuration Changed Event
```

The Controller SHALL apply configuration changes atomically.

---

# Dynamic Configuration

Configuration MAY be modified during runtime unless explicitly prohibited.

Configuration changes SHALL NOT require Controller restart unless otherwise specified.

---

# Resource Representation

Configuration SHALL be exposed through Controller-owned Resources.

Examples include:

- system configuration;
- route configuration;
- Unit configuration.

The Resource Namespace SHALL remain the only externally visible representation of configuration.

---

# Persistence

Configuration SHALL persist across Controller restarts.

The Configuration Model intentionally makes no assumptions regarding:

- file formats;
- databases;
- storage mechanisms.

Persistence mechanisms are defined elsewhere in the specifications.

---

# Ownership

The Configuration Service SHALL own:

- configuration state;
- configuration validation;
- configuration persistence coordination.

No other Service SHALL directly modify configuration.

---

# Design Principles

The Configuration Model SHALL satisfy the following principles.

- Explicit ownership.
- Persistent configuration.
- Transport independence.
- Hardware independence.
- Validation before application.
- Runtime configurability.
- Controller-owned resources.