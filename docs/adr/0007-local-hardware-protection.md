# ADR-0007: Local Hardware Protection

**Status:** Accepted

## Context

The OpenExtraction Controller is the sole authority responsible for operational decisions.

However, individual devices directly interface with physical hardware and are therefore best positioned to detect and respond to hardware-specific conditions.

Examples include:

- A blast gate already being fully open.
- A motor reaching a limit switch.
- A movement timeout.
- Excessive motor current.
- A failed position sensor.
- A locally activated hardware inhibit switch.

Allowing the Controller to directly manage these hardware details would unnecessarily couple Controller firmware to individual device implementations and reduce system extensibility.

## Decision

The Controller SHALL decide *what* the system should do.

Individual devices SHALL decide *how* to safely execute Controller commands.

Devices SHALL refuse or abort commands that would place local hardware in an unsafe or undefined state.

Devices SHALL report abnormal conditions to the Controller.

Local safety decisions SHALL NOT modify workshop topology, routing, scheduling or extraction policy.

The Controller SHALL treat reported hardware faults as events and determine any required system-level response.

## Examples

Examples of local hardware protection include:

- Ignoring an OPEN command when the gate is already fully open.
- Stopping motor movement when a limit switch is reached.
- Aborting movement after a configurable timeout.
- Detecting excessive motor current and reporting a fault.
- Preventing operation while a local hardware inhibit input is active.

Examples that are NOT local hardware protection include:

- Deciding whether extraction is required.
- Selecting which blast gates should open.
- Starting or stopping the dust collector.
- Modifying extraction routes.
- Reconfiguring workshop topology.

## Consequences

Advantages:

- Clear separation of responsibilities.
- Simpler Controller implementation.
- Hardware-independent Controller logic.
- Easier support for different actuator technologies.
- Improved fault isolation.
- Increased system robustness.

Disadvantages:

- Device firmware becomes responsible for additional safety logic.
- Devices must implement hardware-specific fault detection.

These trade-offs are considered acceptable because they improve reliability while preserving the Controller as the single authority for operational decisions.