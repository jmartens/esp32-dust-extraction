# Runtime

## Purpose

This document defines the runtime behaviour and lifecycle of the OpenExtraction Controller.

The Runtime Model specifies:

- Controller startup behaviour;
- service initialisation;
- automatic operation;
- runtime operation;
- shutdown behaviour;
- fault recovery behaviour.

The Runtime Model intentionally does not specify Physical Device firmware behaviour.

---

# Principles

The Runtime Model is founded upon the following principles.

- Runtime behaviour SHALL be deterministic.
- Services SHALL be initialised in a defined order.
- Automatic operation SHALL NOT begin until startup is complete.
- Runtime behaviour SHALL remain transport independent.
- Runtime behaviour SHALL remain hardware independent.

---

# Controller Lifecycle

The Controller lifecycle consists of the following phases.

```text
Startup

↓

Initialisation

↓

Operational

↓

Maintenance

↓

Shutdown
```

---

# Startup

The Startup phase begins immediately after power is applied or the Controller process is started.

The Controller SHALL:

- initialise Infrastructure Adapters;
- initialise Infrastructure Services;
- initialise Domain Services;
- restore persistent information.

Automatic operation SHALL remain disabled during Startup.

---

# Initialisation

During Initialisation the Controller SHALL:

- validate configuration;
- initialise the Resource Namespace;
- initialise Controller Services;
- prepare the Event Queue;
- prepare command dispatching.

The Controller SHALL NOT process automatic extraction requests during this phase.

---

# Operational

The Operational phase represents normal Controller behaviour.

The Controller SHALL:

- process Domain Events;
- dispatch Commands;
- maintain Controller state;
- perform routing decisions;
- expose Controller Resources.

Automatic operation SHALL be enabled during this phase unless explicitly disabled.

---

# Maintenance Mode

Maintenance Mode temporarily disables automatic operation.

The Controller SHALL continue to:

- process Events;
- expose Resources;
- accept configuration changes;
- expose diagnostics information.

Automatic extraction behaviour SHALL be suspended.

Manual operations MAY remain permitted.

---

# Shutdown

During Shutdown the Controller SHALL:

- complete critical persistence operations;
- stop automatic operation;
- gracefully terminate Services where possible.

Runtime information SHALL NOT necessarily be persisted.

---

# Automatic Operation

Automatic operation SHALL begin only after:

- startup has completed;
- configuration has been validated;
- required Services have been initialised.

Automatic operation SHALL be disabled during:

- startup;
- maintenance mode;
- shutdown.

---

# Event Processing

During runtime the Controller SHALL:

- process Domain Events sequentially;
- preserve event ordering;
- guarantee deterministic behaviour.

The Event Dispatcher SHALL own the Event Queue.

---

# Device Discovery

Physical Devices MAY be discovered at any time during runtime.

Discovery SHALL NOT:

- imply trust;
- automatically enable automation;
- modify configuration.

Discovery SHALL remain independent of automatic operation.

---

# Runtime Configuration

Configuration changes MAY occur during runtime.

The Controller SHALL:

- validate changes;
- apply changes atomically;
- persist changes when required.

Controller restart SHALL NOT normally be required following configuration changes.

---

# Fault Recovery

The Controller SHALL continue operating where possible following faults.

Examples include:

- unavailable Routes;
- offline Physical Devices;
- failed Commands.

Fault recovery SHALL be performed by the owning Controller Services.

---

# Persistence Restoration

Persistent information SHALL be restored during startup.

Examples include:

- configuration;
- pairing information;
- calibration information.

Runtime state SHALL be reconstructed during normal operation.

---

# Service Lifecycle

Controller Services SHALL:

- be created during startup;
- remain active throughout runtime;
- participate in shutdown procedures where applicable.

Services SHALL NOT be dynamically created or destroyed.

---

# Resource Availability

The Resource Namespace SHALL accurately reflect the current Controller state.

Resources MAY become available or unavailable during runtime.

Examples include:

- newly discovered Physical Devices;
- newly configured Routes;
- maintenance mode activation.

---

# Physical Devices

Physical Devices MAY:

- join the network;
- leave the network;
- become temporarily unavailable.

The Controller SHALL maintain deterministic behaviour regardless of Physical Device availability.

---

# Runtime States

The Controller SHALL operate in one of the following runtime states.

```text
Startup

↓

Initialisation

↓

Operational

↓

Maintenance

↓

Operational

↓

Shutdown
```

Transitions SHALL be deterministic.

---

# Design Principles

The Runtime Model SHALL satisfy the following principles.

- Deterministic runtime behaviour.
- Explicit lifecycle management.
- Transport independence.
- Hardware independence.
- Predictable startup and shutdown.
- Separation of startup and automatic operation.
- Safe fault recovery.