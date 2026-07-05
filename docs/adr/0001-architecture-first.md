# ADR-0001: Architecture First Development

**Status:** Accepted

## Context

Embedded projects often evolve by adding features directly to firmware. As the number of devices and interactions grows, the resulting code becomes difficult to maintain, document and test.

OpenExtraction is intended to become a long-lived open-source project with multiple device types and contributors.

## Decision

The project shall follow an Architecture First Development approach.

The implementation shall follow a documented architecture rather than defining it.

All major architectural changes shall be documented as Architecture Decision Records (ADRs) before implementation begins.

## Consequences

### Positive

- Consistent architecture.
- Easier onboarding of contributors.
- Better documentation.
- Easier AI-assisted development.
- Stable interfaces.

### Negative

- More documentation effort up front.
- Slower initial implementation.

The project considers these trade-offs worthwhile.
