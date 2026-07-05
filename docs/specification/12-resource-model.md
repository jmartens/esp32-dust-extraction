# Resource Model

## Purpose

This document defines the Controller-owned Resource Namespace exposed by the OpenExtraction architecture.

Resources provide a transport-independent representation of Controller-owned information.

The Resource Model is independent of:

- communication transports;
- external APIs;
- message formats;
- storage mechanisms;
- user interfaces.

Resources SHALL be owned exclusively by the Controller.

---

# Principles

The Resource Model is founded upon the following principles.

- Resources are projections of information.
- Resources SHALL NOT own information.
- Resources SHALL remain transport independent.
- Resources SHALL remain hardware independent.
- Resources SHALL be deterministic.
- Resources SHALL expose Controller-owned information only.

---

# Resource Namespace

The Controller SHALL expose a Resource Namespace.

Resources SHALL provide access to information owned by Controller Services.

Examples include:

- System resources
- Device resources
- Unit resources
- Route resources
- Configuration resources
- Statistics resources
- Diagnostic resources

The Resource Namespace SHALL represent the complete externally visible state of the Controller.

---

# Resource Ownership

Resources SHALL NOT own information.

Information ownership SHALL remain with the owning Service.

Examples:

| Resource | Information Owner |
|---------|---------|
| Devices | Device Registry |
| Routes | Routing Service |
| Configuration | Configuration Service |
| Statistics | Diagnostics Service |
| Diagnostics | Diagnostics Service |

Resources are read-only projections of Controller-owned information unless explicitly defined otherwise.

---

# System Resources

System Resources represent global Controller information.

Examples include:

```text
/system
/system/state
/system/version
/system/capabilities
```

Examples of information include:

- Controller version
- Controller status
- Controller capabilities
- maintenance mode state

---

# Device Resources

Device Resources represent Physical Devices known to the Controller.

Examples include:

```text
/devices
/devices/{deviceUid}
```

Examples of information include:

- Device UID
- hardware information
- firmware version
- pairing state
- online state

---

# Unit Resources

Unit Resources represent logical Units hosted by Physical Devices.

Examples include:

```text
/units
/units/{unitId}
```

Examples of information include:

- Unit Type
- Capabilities
- Reported State
- metadata

The Controller SHALL expose Units independently of their hosting Physical Device.

---

# Route Resources

Route Resources represent Controller routing information.

Examples include:

```text
/routes
/routes/{routeId}
```

Examples of information include:

- Route definition
- active state
- participating Units

---

# Configuration Resources

Configuration Resources expose configuration information.

Examples include:

```text
/configuration
/configuration/system
/configuration/routes
/configuration/units
```

Configuration Resources SHALL expose persistent configuration information only.

---

# Statistics Resources

Statistics Resources expose operational statistics.

Examples include:

```text
/statistics
/statistics/system
/statistics/units
```

Examples include:

- runtime counters
- start counters
- communication counters

---

# Diagnostic Resources

Diagnostic Resources expose diagnostic information.

Examples include:

```text
/diagnostics
/diagnostics/system
/diagnostics/devices
```

Examples include:

- fault history
- health information
- communication diagnostics

---

# Resource Categories

Resources SHALL expose information belonging to one or more Information Model categories.

Examples:

| Category | Example Resource |
|--------|--------|
| Reported State | /units |
| Controller State | /routes |
| Configuration | /configuration |
| Metadata | /devices |
| Statistics | /statistics |
| Diagnostics | /diagnostics |

---

# Resource Relationships

Resources MAY reference one another.

Examples:

```text
Route

↓

required Units


Unit

↓

hosting Physical Device


Physical Device

↓

hosted Units
```

Resources SHALL NOT duplicate ownership of information.

---

# Resource Lifetime

The lifetime of a Resource SHALL be determined by the lifetime of the underlying information.

Examples:

- Unit Resources exist while the Unit is known to the Controller.
- Route Resources exist while configured.
- Diagnostic Resources may persist across restarts.

---

# Dynamic Resources

Resources MAY be created or removed dynamically.

Examples include:

- discovery of new Physical Devices;
- addition of new Routes;
- removal of Units.

The Resource Namespace SHALL always accurately represent the current Controller state.

---

# External Interfaces

External interfaces SHALL interact exclusively with Resources.

Examples include:

- REST APIs
- WebSocket APIs
- Home Assistant integrations
- MQTT integrations
- CLI interfaces

External interfaces SHALL NOT directly access:

- Controller Services;
- transport implementations;
- Physical Devices.

---

# Relationship Model

```text
                Domain Model
                       │
                       ▼
                Information Model
                       │
                       ▼
                 Resource Model
                       │
                       ▼
                 Interface Model
                       │
                       ▼
                 Transport Model
```

Resources form the boundary between the Controller's internal architecture and all external interfaces.

---

# Design Principles

The Resource Model SHALL satisfy the following principles.

- Explicit ownership.
- Transport independence.
- Hardware independence.
- Controller-owned resources.
- Deterministic behaviour.
- Clear separation of concerns.
- Extensibility.