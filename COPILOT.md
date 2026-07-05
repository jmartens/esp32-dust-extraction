# GitHub Copilot Instructions

## Purpose

You are contributing to **OpenExtraction**, a distributed embedded control system for automated dust extraction in woodworking workshops.

This project is architecture-first.

Implementation shall follow the project specifications and Architecture Decision Records (ADRs).

If implementation and documentation disagree, **documentation is authoritative**.

---

# Before Writing Code

Always read, in order:

1. ARCHITECTURE.md
2. README.md
3. docs/adr/
4. docs/specification/

Never invent architecture that contradicts accepted ADRs.

If a requested implementation appears to conflict with an ADR, explain the conflict rather than silently implementing it.

---

# Project Goals

The system must be:

- deterministic
- testable
- transport independent
- hardware independent
- maintainable
- extensible

Correctness is more important than implementation convenience.

---

# Architecture Summary

The system consists of:

- Controller
- Machine Nodes
- Gate Nodes
- Dust Collector

The Controller is the only authority that makes operational decisions.

Machine Nodes detect extraction demand.

Gate Nodes operate blast gates.

The Dust Collector starts only when instructed by the Controller.

---

# Controller Authority

The Controller owns:

- routing
- scheduling
- collector control
- workshop configuration
- device registry

Machine Nodes SHALL NOT:

- know routing
- command gates
- start collectors
- know workshop topology

Gate Nodes SHALL NOT:

- know machines
- know routes
- make operational decisions

---

# Configuration Ownership

The Controller is the single source of truth.

Configuration includes:

- routes
- timings
- friendly names
- security
- pairing
- device assignments

Devices store only information required for their own operation.

---

# Device Identity

Every Physical Device SHALL expose an immutable Device UID.

The architecture intentionally makes no assumptions regarding the UID representation.

Friendly names are configuration only and SHALL NOT be used as identifiers.

---

# Event Driven Core

Business logic is event driven.

External inputs are translated into Events.

Business logic consumes Events.

Business logic emits Commands.

Transport layers translate Commands into protocol messages.

Business logic never interacts directly with:

- ESP-NOW
- Wi-Fi
- HTTP
- GPIO
- NVS
- ESP-IDF

---

# Events

Events represent facts.

Examples:

- MachineDemandStarted
- MachineDemandStopped
- GateOpened
- GateClosed
- CollectorStarted
- CollectorStopped
- DeviceOffline

Events:

- are immutable
- never request behaviour
- may have multiple subscribers

---

# Commands

Commands represent requests.

Examples:

- OpenGate
- CloseGate
- StartCollector
- StopCollector
- PairDevice

Commands:

- have one recipient
- never modify Controller state directly
- are confirmed by later Events

---

# Controller Services

The Controller consists of independent services.

Examples include:

- Configuration Service
- Routing Service
- Device Registry
- Diagnostics Service
- Event Dispatcher
- Command Dispatcher

Additional Services MAY be introduced where required.

Services SHALL NOT invoke behavioural methods on other services.

Read-only queries are permitted.

---

# Transport Layer

The transport layer is responsible for:

- ESP-NOW
- packet encoding
- packet decoding
- authentication
- replay protection
- retransmission
- message acknowledgement

The transport layer SHALL NOT contain business logic.

---

# Hardware Layer

Drivers abstract hardware.

Business logic shall never manipulate GPIO directly.

Examples:

Correct:

CollectorRelay.start()

Incorrect:

digitalWrite(RELAY_PIN, HIGH);

---

# Routing

Routes belong exclusively to the Controller.

Routes map:

Machine Unit

↓

Route

↓

Required Units

# Routing

Routing is exclusively owned by the Controller.

Routes describe the relationship between logical Units.

The Routing Service computes the union of all active Routes.

Shared Units SHALL remain active while required by one or more active Routes.

Physical Devices SHALL NOT participate in routing decisions.

---

# Local Hardware Protection

Nodes protect their own hardware.

Examples:

- limit switches
- movement timeout
- motor current monitoring
- inhibit switch

Nodes may reject unsafe Commands.

Nodes never modify workshop policy.

---

# State Machines

Represent behaviour using explicit state machines.

Avoid:

- boolean flag combinations
- deeply nested if statements
- implicit state

Prefer:

enum class State

over multiple booleans.

---

# Architectural Layers

The architecture follows the following layered model:

Domain Model

↓

Information Model

↓

Resource Model

↓

Interface Model

↓

Transport Model

Lower layers SHALL NOT influence higher layers.

Business logic SHALL reside exclusively within the Domain Model and Controller Services.

Transport and interface concerns SHALL remain isolated.

---


# Units and Capabilities

Physical Devices host one or more logical Units.

Examples include:

- Machine Units
- Gate Units
- Collector Units

Units expose one or more Capabilities.

Examples include:

- Current Measurement
- Gate Actuation
- Position Feedback
- Collector Control

Configuration, diagnostics, resources and behaviour are determined by Unit Type and Capabilities.

Business logic SHALL operate on Units rather than Physical Devices wherever possible.

---

# Resource Model

The Controller exposes a Controller-owned Resource Namespace.

Examples include:

- Devices
- Units
- Routes
- Configuration
- Diagnostics
- Statistics

Resources are projections of Controller-owned information.

Resources SHALL NOT own information.

External interfaces SHALL interact exclusively through Resources.

---

# Faults and Errors

Faults are business-significant domain concepts.

Examples include:

- RouteUnavailable
- GateFault
- CollectorFault

Errors are implementation concerns.

Examples include:

- malformed packets
- transport failures
- storage failures

Faults SHALL be represented as Domain Events.

Errors SHALL be handled locally unless elevated to Faults.

---

# Persistence

Persistence is a policy concern.

The architecture intentionally makes no assumptions regarding:

- storage formats
- filesystems
- databases

Persistent information is owned by Controller Services.

Runtime information SHALL NOT be persisted unless explicitly specified.

---

# C++ Guidelines

Use C++20 where supported.

Prefer:

- constexpr
- enum class
- std::optional
- std::variant
- std::span
- RAII
- composition
- dependency injection

Avoid:

- macros
- globals
- singleton patterns
- raw owning pointers
- magic numbers

---

# Error Handling

Recover where possible.

Generate fault Events.

Never silently ignore failures.

---

# Testing

Business logic should compile as native desktop C++.

Business logic should be unit-testable without ESP hardware.

Mock:

- transport
- timers
- GPIO
- storage

Avoid mocking business logic.

---

# Documentation

When introducing new behaviour:

- update the specification
- update architecture documentation if needed
- create or update an ADR for architectural changes

Do not modify architecture without updating documentation.

---

# Repository Conventions

Prefer the ESP-IDF component model.

Avoid placing business logic in:

- main/
- transport adapters
- hardware drivers

Business logic belongs in dedicated domain components.

Keep components small and focused.

---

# Code Generation Preferences

Prefer small focused classes.

Keep functions short.

Keep public interfaces minimal.

Prefer composition over inheritance.

Avoid premature optimisation.

Prioritise readability.

Prefer:

- constexpr
- enum class
- std::optional
- std::variant
- std::span
- std::string_view
- RAII
- composition
- dependency injection
- strong types

Avoid:

- macros
- globals
- singletons
- raw owning pointers
- magic numbers
- exceptions for control flow
- hidden state
- cyclic dependencies

---

# Development Tooling

The project uses:

- ESP-IDF
- CMake
- pre-commit
- clang-format
- clang-tidy
- cppcheck
- semantic versioning
- conventional commits
- semantic-release
- GitHub Actions

Generated code and examples SHALL conform to these tools and workflows.

Never optimise for reduced flash or RAM usage unless explicitly requested.

Correctness, readability and maintainability are preferred over micro-optimisations.

Premature optimisation is discouraged.

---

# If Unsure

When several implementations are possible:

1. Follow the ADRs.
2. Prefer deterministic behaviour.
3. Prefer explicit state machines.
4. Prefer events over direct calls.
5. Prefer Commands over side effects.
6. Preserve transport independence.
7. Preserve Controller authority.

When in doubt, ask for clarification rather than inventing new architecture.