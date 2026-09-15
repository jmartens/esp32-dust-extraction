# OpenExtraction Documentation

This directory contains the architecture, specifications, decisions, and
implementation guidance for OpenExtraction.

## Start Here

- [Project Vision](00-project-vision.md)
- [System Architecture](01-system-architecture.md)
- [Design Principles](02-design-principles.md)
- [Glossary](03-glossary.md)
- [Roadmap](04-roadmap.md)

## Architecture Decisions

The [ADR directory](adr/) contains accepted architectural decisions and the
rationale behind the system's major boundaries and ownership rules.

## Specifications

The [specification directory](specification/) contains the authoritative
behavioral and protocol documentation.

## Implementation Guidance

- [Build System](implementation/build-system.md)
- [Coding Standard](implementation/coding-standard.md)
- [Repository Layout](implementation/repository-layout.md)
- [Testing](implementation/testing.md)

## Documentation Authority

Specifications define required behavior. ADRs define accepted architectural
decisions. `ARCHITECTURE.md` and `CONTRIBUTING.md` provide repository-level
guidance. When implementation and documentation disagree, follow the
specifications and accepted ADRs, then resolve the discrepancy explicitly.
