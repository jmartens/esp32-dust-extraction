# ADR-0002: Controller is the Sole Authority

**Status:** Accepted

## Context

Distributed systems can either make local decisions or centralize decision making.

Local decision making increases complexity and can lead to inconsistent system state.

## Decision

The Controller is the only component permitted to make routing decisions.

Machine nodes may only request extraction.

Gate nodes may only execute commands.

Neither machine nor gate nodes shall contain knowledge of workshop topology.

## Consequences

Advantages include:

- deterministic behaviour
- simpler firmware
- easier testing
- easier replacement of devices

The controller becomes the single source of truth.
