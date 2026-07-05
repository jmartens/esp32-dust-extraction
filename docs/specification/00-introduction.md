# OpenExtraction Architecture

## Purpose

OpenExtraction is an event-driven dust extraction control system intended for woodworking workshops.

The system automatically coordinates dust extraction by monitoring woodworking machines, determining the required extraction path, controlling blast gates and operating one or more dust collectors.

The architecture is designed to remain independent of specific hardware, communication transports and user interfaces while providing deterministic and maintainable behaviour.

This documentation defines the architecture, terminology and behaviour of the OpenExtraction system.

---

# Scope

This documentation specifies:

- the domain model;
- the Controller architecture;
- communication principles;
- discovery;
- routing;
- state management;
- messaging;
- configuration;
- diagnostics;
- security.

Implementation details are intentionally excluded unless required to define observable behaviour.

---

# Architecture Overview

The OpenExtraction architecture consists of four conceptual layers.

```text
                    User Interfaces
            (Web UI, REST API, Automation)

                           │

                    Resource Namespace

                           │

                      Controller

        ┌──────────────────────────────────┐
        │                                  │
        │  Device Registry                 │
        │  Routing Service                 │
        │  Gate Service                    │
        │  Collector Service               │
        │  Configuration Service           │
        │  Diagnostics                     │
        │                                  │
        └──────────────────────────────────┘

                           │

               Commands / Domain Events

                           │

                 Transport Abstraction

                           │

                 Physical Devices (ESP)

                           │

                          Units
```

The Controller is the single source of truth.

The Controller is also the single source of authority for all business decisions.

The Controller maintains a declarative model of the workshop and its logical topology, including Physical Devices, Units, Routes, configuration and associated metadata.

Business logic executes exclusively within the Controller.

Physical Devices execute commands and report events.

---

# Guiding Principles

The OpenExtraction architecture is founded upon the following principles.

## Controller-centric

The Controller SHALL make all business decisions.

Physical Devices SHALL execute commands but SHALL NOT perform routing or automation.

---

## Event-driven

State changes SHALL occur only while processing Domain Events.

Events represent facts.

Commands represent requests.

---

## Transport-independent

Business logic SHALL remain independent of the communication transport.

The transport layer SHALL contain no business logic.

---

## Hardware-independent

Business logic SHALL operate on Units rather than hardware devices.

The physical distribution of Units across hardware SHALL NOT affect Controller behaviour.

---

## Capability-driven

The Controller SHALL determine behaviour using Unit Types and advertised Capabilities.

The Controller SHALL NOT make assumptions based solely on hardware or firmware implementation.

---

## Service-oriented

Each Controller Service SHALL own its internal state.

Services SHALL communicate through Domain Events, Commands and synchronous queries.

Shared mutable state between Services SHALL NOT exist.

---

## Deterministic

Given the same sequence of Events, the Controller SHALL always produce the same sequence of Commands and state transitions.

---

## Extensible

The architecture SHALL permit the introduction of:

- new Unit Types;
- new Capabilities;
- new Controller Services;
- new transports;

without requiring architectural redesign.

---

# Domain Model

The architecture distinguishes between the domain model and the Controller resource model.

The domain model consists of:

- Controller
- Physical Device
- Unit
- Route
- Capability
- State

The Resource Namespace is a Controller-owned projection of the domain model.

Resources provide access to domain information but are not themselves domain objects.

---

# Reading Guide

The documents are intended to be read in the following order.

| Document | Purpose |
|----------|---------|
| 00-introduction.md | Architecture overview and guiding principles |
| 01-terminology.md | Definitions of architectural concepts |
| 02-system-requirements.md | Functional and architectural requirements |
| 03-event-model.md | Events, Commands and event processing |
| 04-state-machines.md | Behaviour of Controller and Units |
| 05-message-protocol.md | Communication protocol |
| 06-routing.md | Route definition and activation |
| 07-controller-services.md | Responsibilities of Controller Services |
| 08-device-information.md | Device and Unit discovery |
| 09-configuration.md | Configuration model |
| 10-security.md | Pairing, authentication and trust |
| 11-web-api.md | External API |
| 12-fault-handling.md | Fault detection and recovery |
| 13-persistence.md | Persistent state |
| 14-diagnostics.md | Diagnostics and health reporting |
| 15-logging.md | Logging model |
| 16-runtime.md | Controller startup and runtime behaviour |

---

# Intended Audience

This documentation is intended for:

- firmware developers;
- Controller developers;
- user interface developers;
- contributors;
- maintainers;
- reviewers;
- AI-assisted development tools.

All architectural decisions are captured in the accompanying Architecture Decision Records (ADRs).

The specifications contained within this documentation are normative unless explicitly stated otherwise.

# Architectural Layers

The OpenExtraction architecture is organised as a layered architecture.

Each layer has a clearly defined responsibility and depends only upon the layers above it.

The architecture intentionally separates business concepts from their representation and transport mechanisms.

The architectural layers are:

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

## Domain Model

The Domain Model defines the business concepts of the system.

Examples include:

- Physical Devices
- Units
- Capabilities
- Routes
- Controller Services
- Domain Events
- Commands

The Domain Model is independent of all technical concerns.

---

## Information Model

The Information Model defines what information exists within the system and who owns that information.

Examples include:

- Reported State
- Controller State
- Configuration
- Metadata
- Statistics
- Diagnostics

The Information Model is independent of storage mechanisms and communication protocols.

---

## Resource Model

The Resource Model defines how the Controller exposes information.

Resources are projections of the Information Model and are owned by the Controller.

Examples include:

- configuration resources
- status resources
- statistics resources
- diagnostics resources

The Resource Model is independent of external APIs.

---

## Interface Model

The Interface Model defines how external consumers interact with the Controller.

Examples include:

- REST APIs
- WebSocket APIs
- Web User Interfaces
- future integrations

The Interface Model is independent of transport implementations.

---

## Transport Model

The Transport Model defines how information is transmitted between components.

Examples include:

- ESP-NOW
- Ethernet
- Wi-Fi
- future communication transports

The Transport Model SHALL NOT contain business logic.

---

The OpenExtraction architecture is designed such that changes to lower layers SHALL NOT require modifications to higher layers.

For example, adding support for a new transport SHALL NOT affect the Domain Model or Information Model.