# Contributing to OpenExtraction

Thank you for contributing to OpenExtraction.

OpenExtraction is an architecture-first, event-driven embedded control system for automated dust extraction in woodworking workshops. The project prioritises correctness, determinism, maintainability and extensibility over implementation convenience.

Before contributing, please familiarise yourself with the project architecture and development philosophy.

## Development Philosophy

The project is guided by the following principles:

* Architecture first.
* Deterministic behaviour.
* Explicit ownership of state and responsibilities.
* Transport and hardware independence.
* Testability.
* Maintainability.
* Small, incremental and releasable changes.
* Documentation is authoritative.

Every merged pull request should leave the repository in a releasable state.

---

## Documentation Hierarchy

When contributing, documentation takes precedence over implementation.

The documentation hierarchy is:

1. Architecture specifications (`docs/specification/`)
2. Architecture Decision Records (`docs/adr/`)
3. `ARCHITECTURE.md`
4. `README.md`
5. Source code

If code conflicts with the architecture documentation, the documentation is considered correct unless an accepted ADR states otherwise.

---

## Architectural Changes

Architectural changes require an Architecture Decision Record (ADR).

Before implementing a change, ask the following questions:

* Does this change ownership of information or responsibilities?
* Does this change the event model?
* Does this change service responsibilities?
* Does this change routing behaviour?
* Does this change the resource model?
* Does this change the configuration model?
* Does this change the message protocol?
* Does this introduce new architectural concepts?
* Does this affect Controller authority or trust boundaries?

If the answer to any of the above is "yes", an ADR is required.

The recommended workflow is:

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

Code should never introduce architectural changes without corresponding documentation updates.

---

## Branching Strategy

The project follows a simple GitHub Flow workflow.

```text
main
    ↓
feature branch
    ↓
pull request
    ↓
CI validation
    ↓
merge to main
    ↓
automatic release
```

The `main` branch shall always remain releasable.

Release branches and long-lived development branches are intentionally avoided.

---

## Versioning

The project follows Semantic Versioning.

Examples:

```text
v0.1.0
v0.2.0
v1.0.0
```

Versions are managed automatically through the release workflow.

Contributors shall never manually update version numbers unless explicitly required by the release tooling.

---

## Conventional Commits

All commits shall follow the Conventional Commits specification.

Examples:

```text
feat(routing): implement route activation logic

fix(configuration): validate duplicate unit IDs

docs(specification): update resource model

refactor(event): simplify event dispatcher

test(service): add collector service unit tests

ci(build): add ESP-IDF build workflow
```

Preferred commit types include:

```text
feat
fix
docs
refactor
test
ci
build
chore
perf
style
```

---

## Pull Requests

Pull requests should be:

* focused;
* small;
* independently reviewable;
* releasable.

Pull requests should not mix unrelated concerns.

Examples of good pull requests:

* Implement Event Dispatcher.
* Add Configuration Service tests.
* Add ESP-IDF build workflow.
* Update Routing specification.

Examples of poor pull requests:

* Implement routing, transport and web API support simultaneously.
* Large architectural refactors without an ADR.

---

## Coding Standards

The project targets modern C++.

Preferred practices:

* C++20 language features.
* RAII.
* composition over inheritance;
* strong typing;
* explicit state machines;
* dependency injection;
* constexpr where appropriate;
* enum class;
* std::optional;
* std::variant;
* std::span;
* std::string_view.

Avoid:

* macros;
* global mutable state;
* singleton patterns;
* raw owning pointers;
* hidden side effects;
* cyclic dependencies;
* magic numbers;
* premature optimisation.

Correctness and maintainability are preferred over micro-optimisations.

---

## Architecture Requirements

Business logic:

* SHALL be event driven.
* SHALL remain transport independent.
* SHALL remain hardware independent.
* SHALL be testable without ESP hardware.
* SHALL follow accepted ADRs and specifications.

Business logic SHALL NOT:

* directly manipulate GPIO;
* perform ESP-NOW operations;
* access transport implementations;
* bypass Controller Services.

The Controller is:

* the single source of truth;
* the single source of authority for business decisions.

---

## Repository Structure

Business logic belongs within dedicated components and services.

Avoid placing business logic in:

```text
main/
hardware adapters/
transport adapters/
```

Repository structure should favour small, focused components.

Examples include:

```text
components/

controller/
event_dispatcher/
command_dispatcher/
routing_service/
configuration_service/
device_registry/
diagnostics_service/
transport/
persistence/
```

---

## Testing Philosophy

Business logic shall be testable on desktop platforms.

Testing layers are:

```text
Unit tests
    ↓
Service tests
    ↓
Integration tests
    ↓
ESP-IDF build tests
    ↓
Hardware-in-the-loop tests
```

Mock:

* transports;
* timers;
* storage;
* hardware interfaces.

Avoid mocking business logic whenever possible.

A feature should be tested at the lowest practical level.

---

## Documentation Requirements

Changes that introduce or modify behaviour shall update the appropriate documentation.

This may include:

* specifications;
* ADRs;
* README;
* examples;
* diagrams.

Documentation should be updated in the same pull request whenever possible.

---

## CI/CD Requirements

All contributions shall pass the project's CI pipeline.

The CI pipeline may include:

* formatting checks;
* linting;
* static analysis;
* unit tests;
* ESP-IDF builds;
* documentation validation;
* release validation.

Code that fails CI shall not be merged.

---

## Development Tooling

The project uses or intends to use:

* ESP-IDF;
* CMake;
* GitHub Actions;
* pre-commit;
* clang-format;
* clang-tidy;
* cppcheck;
* semantic-release;
* Conventional Commits.

Generated code should conform to the project's tooling and workflows.

---

## Copilot Usage

GitHub Copilot shall follow the instructions defined in `COPILOT.md`.

Contributors using AI-assisted development are expected to:

* review generated code;
* verify architectural compliance;
* ensure documentation remains accurate.

AI-generated code is subject to the same quality standards as manually written code.

---

## If You Are Unsure

When in doubt:

1. Read the specifications.
2. Read the relevant ADRs.
3. Prefer deterministic behaviour.
4. Preserve architectural layering.
5. Preserve Controller authority.
6. Prefer explicit ownership.
7. Ask questions before introducing new architecture.

If implementation convenience conflicts with the architecture, the architecture wins.

---

Thank you for helping build OpenExtraction.
