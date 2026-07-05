# Diagnostics

## Purpose

This document defines the Diagnostics Model of the OpenExtraction Controller.

Diagnostics provide operational visibility into the Controller and its managed resources.

Diagnostics SHALL remain independent of:

- business logic;
- fault handling;
- transport implementations;
- storage mechanisms.

The Diagnostics Service SHALL be observational only.

---

# Principles

The Diagnostics Model is founded upon the following principles.

- Diagnostics are observational.
- Diagnostics SHALL NOT influence business logic.
- Diagnostics SHALL remain transport independent.
- Diagnostics SHALL remain hardware independent.
- Diagnostics information SHALL have explicit ownership.

---

# Diagnostic Information

Diagnostics MAY include:

- health information;
- operational statistics;
- communication statistics;
- fault history;
- resource utilisation;
- timing information.

Diagnostic information SHALL NOT directly influence Controller behaviour.

---

# Health Information

Health information describes the operational status of the Controller.

Examples include:

- Controller running state;
- service health;
- device connectivity;
- resource availability.

Health information SHALL be derived from Controller-owned information.

---

# Statistics

Statistics provide quantitative operational information.

Examples include:

- collector runtime;
- gate cycle count;
- machine runtime;
- command counters;
- event counters.

Statistics MAY be persistent.

---

# Communication Diagnostics

Communication diagnostics provide visibility into transport behaviour.

Examples include:

- messages transmitted;
- messages received;
- retries;
- communication failures.

Communication diagnostics SHALL remain transport independent at the architectural level.

Transport-specific diagnostic information MAY be provided by transport implementations.

---

# Fault History

Diagnostics MAY expose historical fault information.

Examples include:

- active faults;
- resolved faults;
- fault counters;
- fault timestamps.

The Diagnostics Service SHALL NOT own active fault state.

---

# Resource Diagnostics

Resources MAY expose diagnostic information.

Examples include:

```text
/diagnostics/system

/diagnostics/devices

/diagnostics/units

/diagnostics/services
```

The Resource Model SHALL determine how diagnostic information is exposed.

---

# Service Diagnostics

Controller Services MAY expose diagnostic information.

Examples include:

Routing Service:

- active route count;
- route activation statistics.

Collector Service:

- collector start count;
- run-on timer statistics.

Device Registry:

- paired device count;
- online device count.

---

# Timing Diagnostics

Timing diagnostics MAY include:

- event processing times;
- command execution times;
- service execution times.

Timing information SHALL be observational only.

---

# Diagnostic Ownership

Diagnostic information SHALL remain owned by the Diagnostics Service unless otherwise specified.

Services MAY contribute diagnostic information but SHALL NOT own its representation within the diagnostic namespace.

---

# Persistence

Diagnostic information MAY be persistent.

Examples include:

- historical statistics;
- fault history;
- operational counters.

Persistence policies are defined separately.

---

# External Interfaces

Diagnostic information MAY be exposed through Controller-owned Resources.

Examples include:

- Web APIs;
- WebSockets;
- Home Assistant integrations;
- command-line interfaces.

Diagnostic information SHALL NOT expose internal implementation details unless explicitly intended.

---

# Design Principles

The Diagnostics Model SHALL satisfy the following principles.

- Observational only.
- Transport independence.
- Hardware independence.
- Explicit ownership.
- Separation from business logic.
- Separation from fault handling.
- Extensible diagnostic information.