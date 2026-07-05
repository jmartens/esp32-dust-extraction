# ADR-0004: Routing Configuration is Controller-Owned

**Status:** Accepted

## Context

The OpenExtraction system consists of a Controller, Machine Nodes and Gate Nodes.

During the design phase, several alternatives were considered for storing extraction routes:

- Routes stored in Machine Nodes
- Reusable extraction profiles
- Routes stored centrally in the Controller

Storing routing information on Machine Nodes couples firmware to a specific workshop installation. Reusable profiles introduce an additional layer of indirection that provides little benefit for the majority of woodworking shops, where each machine typically has a unique extraction path.

The project has adopted the principle that the Controller is the single source of truth for workshop configuration.

## Decision

The Controller SHALL maintain the complete workshop routing configuration.

Routes SHALL be associated with Machine Device UIDs.

Routes SHALL reference Gate Device UIDs.

Machine Nodes SHALL NOT contain routing information.

Gate Nodes SHALL NOT contain knowledge of which machines require them.

A route MAY contain zero Gate Device UIDs.

An empty route indicates that no controllable blast gates are required for extraction.

## Consequences

Advantages:

- Complete separation of firmware and workshop configuration.
- Replacing a Machine Node does not require reconfiguring routing.
- Replacing a Gate Node only requires updating Controller configuration.
- Workshop topology exists in exactly one location.
- Simpler Machine and Gate firmware.
- Easier diagnostics and maintenance.

Disadvantages:

- The Controller configuration becomes more important.
- The Controller must maintain the complete routing database.

These trade-offs are considered acceptable because the Controller is already the authoritative source of workshop configuration.