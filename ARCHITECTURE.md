# OpenExtraction Architecture Overview

## Purpose

This document provides a high-level overview of the OpenExtraction architecture and serves as an entry point into the project's architectural documentation.

The authoritative architectural documentation is located in:

* `docs/adr/`
* `docs/specification/`

This document intentionally avoids duplicating architectural details that are specified elsewhere.

---

## Architectural Principles

OpenExtraction is:

* architecture first;
* event driven;
* deterministic;
* transport independent;
* hardware independent;
* testable;
* maintainable;
* extensible.

The architecture prioritises correctness and explicit ownership over implementation convenience.

---

## Controller Centric Architecture

The OpenExtraction Controller is:

* the single source of truth;
* the single source of authority for all business decisions.

The Controller maintains a declarative model of the workshop and its logical topology, including:

* Physical Devices;
* Units;
* Routes;
* configuration;
* associated metadata.

Physical Devices never participate in business decisions.

---

## Architectural Layers

The architecture follows a layered model.

```text
Domain Model
    ↓
Information Model
    ↓
Resource Model
    ↓
Interface Model
    ↓
Transport Model
```

Business logic resides exclusively within the Domain Model and Controller Services.

Lower layers SHALL NOT influence higher layers.

---

## Event Driven Architecture

Business logic is event driven.

External inputs are translated into immutable Domain Events.

Controller Services consume Domain Events and emit Commands.

Transport implementations translate Commands into protocol specific messages.

The Controller SHALL NOT couple business logic to:

* ESP-NOW;
* Wi-Fi;
* HTTP;
* GPIO;
* storage implementations;
* ESP-IDF specific APIs.

---

## Physical Devices, Units and Capabilities

Physical Devices host one or more logical Units.

Examples include:

* Machine Units;
* Gate Units;
* Collector Units.

Units expose one or more Capabilities.

Examples include:

* Current Measurement;
* Gate Actuation;
* Position Feedback;
* Collector Control.

Business logic operates primarily on Units rather than Physical Devices.

---

## Routing Model

Routing is exclusively owned by the Controller.

Routes describe relationships between logical Units.

The Routing Service:

* activates Routes;
* determines Route availability;
* computes shared Unit usage.

Physical Devices SHALL NOT participate in routing decisions.

---

## Service Model

The Controller consists of independent Services.

Examples include:

* Event Dispatcher;
* Command Dispatcher;
* Configuration Service;
* Routing Service;
* Device Registry;
* Diagnostics Service.

Services:

* own their state;
* consume Events;
* publish Events;
* emit Commands.

Services SHALL NOT invoke behavioural methods on other Services.

---

## Persistence Model

Persistence is a policy concern.

Persistent information includes, where applicable:

* configuration;
* route definitions;
* pairing information;
* calibration information;
* metadata.

Runtime state is intentionally not persisted unless explicitly specified.

Storage mechanisms are implementation specific.

---

## Fault Handling

Faults are business-significant domain concepts.

Examples include:

* GateFault;
* CollectorFault;
* RouteUnavailable.

Faults are represented as Domain Events.

Errors are implementation concerns and SHALL be handled locally unless elevated to Faults.

---

## Diagnostics

Diagnostics are observational.

Diagnostics SHALL NOT influence business logic.

Examples include:

* health information;
* statistics;
* communication metrics;
* fault history;
* resource utilisation.

Diagnostics exist to provide operational visibility into the Controller.

---

## Runtime Model

The Controller lifecycle consists of:

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

Automatic operation begins only after successful startup and configuration validation.

---

## Repository Documentation

The architectural documentation is organised as follows:

```text
docs/

├── adr/
│
└── specification/
    ├── 00-introduction.md
    ├── 01-terminology.md
    ├── 02-system-requirements.md
    ├── ...
    └── 16-runtime.md
```

Architecture Decision Records document architectural decisions and their rationale.

Specifications describe the authoritative behaviour of the system.

---

## Development Workflow

Architectural changes SHALL follow the following workflow:

```text
Propose ADR
    ↓
Review ADR
    ↓
Accept ADR
    ↓
Update specifications
    ↓
Implement code
```

Code SHALL NOT introduce architectural changes without corresponding documentation updates.

---

## Implementation Guidance

Contributors are expected to:

1. Read this document.
2. Read the relevant ADRs.
3. Read the relevant specifications.
4. Follow the repository's contribution guidelines.
5. Preserve the architectural principles of the project.

When implementation convenience conflicts with the architecture, the architecture takes precedence.
