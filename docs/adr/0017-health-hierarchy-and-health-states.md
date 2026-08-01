# ADR-0017 - Health Hierarchy and Health States

## Status

Accepted

## Context

OpenExtraction requires a deterministic mechanism for determining the
operational health of components and the system as a whole.

Health information is required for:

* OTA validation
* Diagnostics
* Watchdog support
* Remote support
* Future fleet management
* Workshop status reporting

## Decision

Health SHALL be hierarchical.

Components derive their health from the health of the components they
own.

The following health states are defined:

```cpp
enum class Health {
    Healthy,
    Degraded,
    Failed
};
```

Health ownership follows the hierarchy:

```text
System
    ↓
Services
    ↓
Devices
    ↓
Units
    ↓
Drivers
```

Higher-level components SHALL aggregate the health of owned
components.

Examples:

* A Device with one failed Unit SHALL report `Degraded`.
* A Controller with a failed mandatory Service SHALL report `Failed`.
* A Device with all Units healthy SHALL report `Healthy`.


---

## State vs Health

OpenExtraction distinguishes between operational state and health.

State answers:

> What is the component currently doing?

Health answers:

> Is the component capable of fulfilling its responsibilities?

These concerns SHALL be represented independently.

Example:

```cpp
enum class DeviceState {
    Unpaired,
    Pairing,
    Paired,
    Initializing,
    Initialized,
    Updating,
    FactoryResetting,
    Failed
};

enum class Health {
    Healthy,
    Degraded,
    Failed
};

---

## Consequences

### Positive

* Deterministic health reporting.
* OTA validation support.
* Simplified diagnostics.
* Consistent semantics across the system.

### Negative

* Additional implementation effort.
* Health aggregation logic required.

---
