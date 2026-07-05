    # Controller Services

## Purpose

This document defines the responsibilities of the Controller Services that together implement the OpenExtraction architecture.

Controller Services encapsulate business logic and own the state required to fulfil their responsibilities.

The Controller SHALL be composed of one or more Controller Services and supporting Infrastructure Services.

---

# Overview

The OpenExtraction Controller consists of:

### Domain Services

- Device Registry
- Routing Service
- Gate Service
- Collector Service
- Configuration Service
- Diagnostics Service

### Infrastructure Services

- Event Dispatcher
- Command Dispatcher

### Infrastructure Adapters

- Transport Adapter(s)
- Persistent Storage Adapter(s)
- Web API Adapter(s)

---

# Service Responsibilities

Each Service SHALL have exactly one responsibility.

Responsibilities SHALL NOT overlap.

Services SHALL communicate exclusively through:

- Domain Events
- Commands
- synchronous queries

Services SHALL NOT directly manipulate another Service's internal state.

---

# Device Registry

The Device Registry owns:

- Physical Device inventory
- Unit inventory
- Device lifecycle state
- Unit ownership information

The Device Registry SHALL:

- discover Physical Devices
- maintain Device UIDs
- maintain Unit information
- track online and offline state
- provide Unit lookup facilities

The Device Registry SHALL NOT perform routing decisions.

---

# Routing Service

Routing Service owns:

- Route activation state
- Gate reference counts
- Route availability

The Routing Service SHALL:

- activate Routes
- deactivate Routes
- determine required Gate Units
- determine Route availability

The Routing Service SHALL NOT generate transport-specific commands.

---

# Gate Service

The Gate Service owns:

- Gate Unit state
- pending Gate operations

The Gate Service SHALL:

- coordinate Gate opening
- coordinate Gate closing
- wait for Gate state transitions
- generate OpenGate Commands
- generate CloseGate Commands

The Gate Service SHALL NOT determine which Gates are required by a Route.

---

# Collector Service

The Collector Service owns:

- Collector lifecycle state
- run-on timers
- Collector coordination logic

The Collector Service SHALL:

- determine when Collectors may start
- determine when Collectors may stop
- manage run-on behaviour
- coordinate Collector operations

The Collector Service SHALL NOT determine Route activation.

---

# Configuration Service

The Configuration Service owns:

- persistent configuration
- route definitions
- configuration validation
- configuration lifecycle management

The Configuration Service SHALL:

- load configuration during startup
- validate configuration changes
- persist configuration changes
- provide configuration queries

The Configuration Service SHALL NOT implement business logic unrelated to configuration.

---

# Diagnostics Service

The Diagnostics Service owns:

- diagnostic state
- health information
- fault history

The Diagnostics Service SHALL:

- collect diagnostics information
- expose health information
- record faults
- expose statistics

The Diagnostics Service SHALL NOT modify business state.

---

# Event Dispatcher

The Event Dispatcher owns:

- the Event Queue

The Event Dispatcher SHALL:

- dispatch Domain Events
- preserve event ordering
- prevent recursive dispatch
- guarantee deterministic event processing

The Event Dispatcher SHALL NOT contain business logic.

---

# Command Dispatcher

The Command Dispatcher owns:

- command delivery

The Command Dispatcher SHALL:

- determine destination Physical Devices
- serialize Commands
- dispatch Commands
- process acknowledgements

The Command Dispatcher SHALL NOT perform business decisions.

---

# Transport Adapters

Transport Adapters provide communication between the Controller and Physical Devices.

Examples include:

- ESP-NOW
- Ethernet
- Wi-Fi

Transport Adapters SHALL:

- encode messages
- decode messages
- report received Domain Events

Transport Adapters SHALL NOT implement business logic.

---

# Persistent Storage Adapters

Persistent Storage Adapters provide storage services.

Examples include:

- JSON files
- SQLite
- NVS
- external databases

Persistent Storage Adapters SHALL:

- persist data
- retrieve data
- remain independent of business logic

---

# Web API Adapters

Web API Adapters expose Controller functionality.

Examples include:

- REST APIs
- WebSockets
- future protocol integrations

Web API Adapters SHALL:

- expose Controller-owned Resources
- translate API requests into Domain Events or synchronous queries
- remain independent of business logic

---

# Service Dependencies

The following dependency hierarchy SHALL be maintained.

```text
Domain Model

↑

Domain Services

↑

Infrastructure Services

↑

Infrastructure Adapters

↑

Transport / Operating System
```

Business logic SHALL exist exclusively within Domain Services.

---

# Controller Startup

During startup the Controller SHALL:

1. initialise Infrastructure Adapters;
2. initialise Infrastructure Services;
3. initialise Domain Services;
4. load persistent configuration;
5. initialise Device Registry;
6. begin Event processing.

The Controller SHALL NOT begin automatic operation until startup has completed successfully.

---

# Design Principles

The Controller Service architecture SHALL satisfy the following principles.

- Single Responsibility.
- Explicit ownership.
- Deterministic behaviour.
- No shared mutable state.
- Transport independence.
- Hardware independence.
- Event-driven communication.
- Clear separation of business logic and infrastructure.