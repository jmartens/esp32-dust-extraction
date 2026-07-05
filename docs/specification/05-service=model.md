# Service Model

## Purpose

This document defines the architectural model for Controller Services.

A Controller Service is the fundamental building block of the OpenExtraction Controller.

Services encapsulate business logic, own state, communicate through Domain Events and Commands, and together implement the behaviour of the Controller.

---

# Principles

The Service Model is founded upon the following principles.

- Every Service has a clearly defined responsibility.
- Every piece of state has exactly one owner.
- Services communicate through well-defined interfaces.
- Business logic is encapsulated within Services.
- Services remain independent of communication transports.
- Services remain deterministic.

---

# Definition

A Controller Service is a long-lived software component responsible for a single area of the domain.

A Service SHALL:

- own business logic;
- own its internal state;
- consume Domain Events;
- produce Domain Events;
- issue Commands;
- answer synchronous queries.

A Service SHALL NOT:

- modify another Service's state;
- depend on transport implementations;
- directly access hardware;
- perform unrelated responsibilities.

---

# Service Categories

OpenExtraction distinguishes between two categories of Controller Services:

- Domain Services
- Infrastructure Services

Both are Services and follow the same architectural rules.

The distinction exists to separate business logic from technical implementation.

---

## Domain Services

Domain Services implement the business behaviour of the OpenExtraction system.

Domain Services own business state.

Domain Services consume Domain Events and produce Domain Events and Commands.

Examples include:

- Device Registry
- Routing Service
- Gate Service
- Collector Service
- Configuration Service
- Diagnostics Service

Domain Services SHALL contain all business rules.

---

## Infrastructure Services

Infrastructure Services provide technical capabilities used by Domain Services.

Infrastructure Services SHALL NOT contain business logic.

Infrastructure Services MAY:

- dispatch Events;
- dispatch Commands;
- encode protocol messages;
- communicate with Physical Devices;
- persist data;
- expose Web APIs.

Examples include:

- Event Dispatcher
- Command Dispatcher
- ESP-NOW Transport Adapter
- HTTP Server
- Persistent Storage

Infrastructure Services SHALL remain reusable.

Infrastructure Services SHALL NOT make business decisions.

---

## Relationship

Infrastructure Services support Domain Services.

Domain Services SHALL NOT depend upon transport-specific implementations.

Infrastructure Services SHALL NOT depend upon business rules.

The dependency direction SHALL always be:

Infrastructure

↓

Domain Services

↓

Domain Model

---

# Responsibilities

Each Service SHALL have one clearly defined responsibility.

Examples include:

| Service | Responsibility |
|----------|----------------|
| Device Registry | Physical Device inventory |
| Routing Service | Route activation |
| Gate Service | Gate coordination |
| Collector Service | Collector operation |
| Configuration Service | Persistent configuration |
| Diagnostics Service | Diagnostics |
| Event Dispatcher | Event distribution |
| Command Dispatcher | Command delivery |

Responsibilities SHALL NOT overlap.

---

# State Ownership

Every item of business state SHALL have exactly one owning Service.

Only the owning Service MAY modify that state.

Other Services SHALL obtain information through:

- Domain Events;
- synchronous queries.

Example:

```text
Routing Service

owns

Route activation
```

The Gate Service SHALL NOT directly modify Route activation.

---

# Domain Events

Services consume Domain Events.

A Service MAY emit zero or more Domain Events while processing another Domain Event.

Domain Events SHALL represent business-significant facts.

Infrastructure state SHALL NOT be published as Domain Events.

---

# Commands

Services MAY issue Commands.

Commands request another component to perform an action.

Commands SHALL NOT modify Controller state.

Controller state SHALL change only after processing subsequent Domain Events.

---

# Synchronous Queries

Services MAY expose synchronous query interfaces.

Queries SHALL:

- return current state;
- NOT modify state;
- execute synchronously.

Examples:

```
IsGateOpen()

GetRoute()

GetCollectorState()

FindUnit()
```

Queries SHALL NOT produce Domain Events.

---

# Internal State

Internal state belongs exclusively to the owning Service.

Examples include:

Routing Service

- active Routes
- Gate reference counts

Gate Service

- Gate states
- pending Commands

Collector Service

- collector state
- run-on timer

Configuration Service

- configuration database

---

# Infrastructure Services

Some Services exist solely to support the architecture.

Infrastructure Services include:

- Event Dispatcher
- Command Dispatcher
- Transport Adapter

Infrastructure Services SHALL NOT contain business logic.

---

# Event Dispatcher

The Event Dispatcher owns the Event Queue.

The Event Dispatcher SHALL:

- preserve Event ordering;
- dispatch Domain Events;
- prevent recursive dispatch;
- collect newly generated Events;
- guarantee deterministic execution.

The Event Dispatcher SHALL NOT contain business logic.

---

# Command Dispatcher

The Command Dispatcher delivers Commands.

The Command Dispatcher SHALL:

- determine destination Physical Device;
- serialize Commands;
- invoke the Transport Adapter;
- track pending acknowledgements.

The Command Dispatcher SHALL NOT perform routing decisions.

---

# Transport Adapter

Transport Adapters isolate the Controller from transport-specific implementations.

Examples include:

- ESP-NOW
- Ethernet
- Wi-Fi

Transport Adapters SHALL:

- encode messages;
- decode messages;
- report received Domain Events.

Transport Adapters SHALL NOT implement business logic.

---

# Resource Namespace

The Controller owns the Resource Namespace.

Services expose portions of the domain model through Controller-owned Resources.

Resources are projections of Service-owned state.

Example:

```text
Configuration Service

↓

Configuration State

↓

Resource Namespace

↓

/devices/{deviceUid}/units/{unitId}/config
```

Services SHALL NOT own Resources.

---

# Dependencies

Dependencies between Services SHALL remain acyclic.

A Service MAY depend upon:

- Domain Events;
- synchronous queries.

A Service SHALL NOT directly depend upon another Service's implementation.

---

# Lifecycle

Services are created during Controller startup.

Services remain active throughout the Controller lifetime.

Services SHALL NOT be dynamically created or destroyed.

---

# Error Handling

Services SHALL validate all received Domain Events.

Unexpected Events SHALL be ignored or logged.

Errors within one Service SHALL NOT corrupt another Service.

---

# Design Principles

The Service Model SHALL satisfy the following principles.

- Single Responsibility.
- Explicit ownership.
- Event-driven communication.
- Deterministic behaviour.
- Transport independence.
- Hardware independence.
- No shared mutable state.
- Clear separation of business logic and infrastructure.