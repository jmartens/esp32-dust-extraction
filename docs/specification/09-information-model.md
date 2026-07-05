# Information Model

## Purpose

This document defines the information model of the OpenExtraction architecture.

The Information Model specifies:

- what information exists within the system;
- who owns that information;
- how information is categorised;
- how information is exposed by the Controller.

The Information Model is independent of:

- communication transports;
- message formats;
- persistent storage;
- external APIs.

---

# Principles

The Information Model is founded upon the following principles.

- Every piece of information has exactly one owner.
- Information ownership SHALL be explicit.
- Information SHALL be transport independent.
- Information SHALL be hardware independent.
- Information SHALL be exposed through Controller-owned Resources.
- Business logic SHALL remain independent of information representation.

---

# Information Categories

Information within the OpenExtraction architecture belongs to one of the following categories.

- Reported State
- Controller State
- Configuration
- Metadata
- Statistics
- Diagnostics

These categories are mutually exclusive.

---

# Information Ownership

Information SHALL have exactly one owner.

Examples include:

| Information | Owner |
|------------|------------|
| Device UID | Device Registry |
| Route activation | Routing Service |
| Route definition | Configuration Service |
| Gate reference count | Routing Service |
| Collector run-on timer | Collector Service |
| Friendly names | Configuration Service |
| Current measurements | Unit |
| Gate position | Unit |
| Runtime statistics | Diagnostics Service |
| Fault history | Diagnostics Service |

Ownership SHALL determine which component is permitted to modify information.

---

# Reported State

Reported State originates from Units hosted by Physical Devices.

Examples include:

Machine Units:

- demand active;
- current measurement;
- fault indication.

Gate Units:

- position;
- moving;
- fault indication.

Collector Units:

- running;
- fault indication.

Reported State SHALL originate exclusively from Units.

The Controller SHALL NOT directly modify Reported State.

---

# Controller State

Controller State represents information derived or maintained by Controller Services.

Examples include:

- Route activation state;
- Gate reference counts;
- run-on timers;
- maintenance mode;
- pending Commands;
- active manual overrides.

Controller State SHALL be owned by exactly one Controller Service.

---

# Configuration

Configuration defines user-configurable behaviour.

Examples include:

- Route definitions;
- timing parameters;
- calibration values;
- current thresholds;
- timeout values;
- friendly names.

Configuration SHALL be owned by the Configuration Service.

Configuration SHALL remain persistent across restarts.

---

# Metadata

Metadata describes Physical Devices and Units.

Examples include:

- Device UID;
- Unit Identifier;
- Unit Type;
- firmware version;
- hardware type;
- descriptions.

Metadata SHALL NOT directly influence business logic unless explicitly defined elsewhere in the specifications.

---

# Statistics

Statistics represent accumulated operational information.

Examples include:

- runtime;
- cycle count;
- number of starts;
- communication counters.

Statistics MAY be derived from Reported State or Controller State.

Statistics SHALL be owned by the Diagnostics Service unless otherwise specified.

---

# Diagnostics

Diagnostics represent health and fault information.

Examples include:

- fault history;
- communication failures;
- timeout events;
- calibration failures.

Diagnostics SHALL remain separate from operational state.

---

# Derived Information

Information MAY be derived from one or more information categories.

Examples include:

```text
Machine Demand Active

+

Route Definition

↓

Route Activated


Gate Position

+

Gate Reference Count

↓

Gate Required


Collector Running

+

Run-On Timer

↓

Collector Active
```

Derived information SHALL NOT duplicate ownership.

Derived information SHALL remain owned by the Service responsible for maintaining it.

---

# Resource Namespace

The Resource Namespace is a Controller-owned projection of the Information Model.

Resources SHALL expose information belonging to one or more information categories.

Examples include:

```text
Configuration

↓

Resource Namespace

↓

/config


Reported State

↓

Resource Namespace

↓

/status


Statistics

↓

Resource Namespace

↓

/statistics


Diagnostics

↓

Resource Namespace

↓

/diagnostics
```

Resources SHALL NOT own information.

---

# Information Lifecycle

Information MAY have different lifetimes.

Examples include:

Reported State:

- transient

Controller State:

- runtime only

Configuration:

- persistent

Metadata:

- persistent

Statistics:

- persistent or transient

Diagnostics:

- implementation dependent

Information lifetime SHALL be defined by the owning Service.

---

# Persistence

Information MAY or MAY NOT be persisted.

Persistence policies SHALL be defined separately.

The Information Model intentionally makes no assumptions regarding storage mechanisms.

---

# External Interfaces

External interfaces SHALL interact exclusively with Controller-owned Resources.

External interfaces SHALL NOT directly access:

- Controller Services;
- Physical Devices;
- transport implementations.

Examples of external interfaces include:

- Web APIs;
- WebSocket interfaces;
- future MQTT integrations.

---

# Relationship Model

```text
                 Domain Model
                        │
                        ▼
                Information Model
                        │
                        ▼
                 Resource Namespace
                        │
                        ▼
                  External APIs
                        │
                        ▼
                    Transports
```

The Information Model forms the bridge between the Domain Model and all external representations of the system.

---

# Design Principles

The Information Model SHALL satisfy the following principles.

- Explicit ownership.
- Transport independence.
- Hardware independence.
- Separation of concerns.
- Deterministic behaviour.
- Clear information lifecycles.
- Controller-owned Resources.
- Extensibility.