# ADR-0003: Immutable Device Identity

**Status:** Accepted

## Context

Earlier designs used installation-specific identifiers such as:

- TS01
- G_MAIN

These identifiers mixed hardware identity with workshop configuration.

## Decision

Every device shall generate its own immutable identity.

The identifier format is:

```
<TYPE>-<MACADDRESS>
```

Examples:

```
MACHINE-7C9EBD002341
GATE-84F3EB12A91C
CONTROLLER-58BF25A77109
```

Friendly names shall be assigned only by the controller.

## Consequences

Advantages:

- automatic discovery
- no factory programming
- simple replacement
- globally unique identifiers

Hardware identity is separated from workshop configuration.
