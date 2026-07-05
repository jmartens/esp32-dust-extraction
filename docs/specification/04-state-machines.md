# State Machines

## Purpose

This document specifies the state machines governing the behaviour of the OpenExtraction Controller.

State machines define the lifecycle of Controller-managed entities and ensure deterministic system behaviour.

State transitions SHALL occur only while processing Domain Events.

---

# Overview

The Controller consists of several independent state machines.

Each state machine SHALL be owned by exactly one Controller Service.

State machines SHALL communicate exclusively through:

- Domain Events
- Commands
- synchronous queries

State machines SHALL NOT directly modify the state owned by another Controller Service.

---

# Controller Services

The following Services own state machines.

| Service | State Machine |
|----------|---------------|
| Device Registry | Physical Device lifecycle |
| Routing Service | Route activation |
| Gate Service | Gate coordination |
| Collector Service | Collector lifecycle |
| Configuration Service | Configuration state |

---

# Device Registry State Machine

The Device Registry tracks the lifecycle of Physical Devices.

```text
Unknown

↓

Discovered

↓

Paired

↓

Online

↓

Offline

↓

Online
```

## Unknown

The Controller has no knowledge of the device.

---

## Discovered

The device has been detected but is not yet trusted.

---

## Paired

The device has been authorised by the Controller.

---

## Online

The device is reachable.

Hosted Units MAY participate in automation.

---

## Offline

Heartbeat timeout or communication failure has occurred.

Hosted Units SHALL NOT receive Commands.

Routes involving unavailable Units SHALL be handled by the Routing Service.

---

# Machine Unit State Machine

Machine Units represent extraction demand.

```text
Idle

↓

Demand Active

↓

Idle
```

---

## Idle

No extraction demand exists.

---

## Demand Active

The Unit requests extraction.

Transition:

MachineDemandStarted

MachineDemandStopped

---

# Gate State Machine

The Gate Service owns the logical state of every Gate Unit.

```text
Closed

↓

Opening

↓

Open

↓

Closing

↓

Closed
```

Faults may occur from any state.

```text
Closed

Open

Opening

Closing

↓

Fault
```

---

## Closed

Gate is fully closed.

---

## Opening

OpenGate Command has been issued.

Waiting for confirmation.

---

## Open

Gate reports fully open.

---

## Closing

CloseGate Command has been issued.

Waiting for confirmation.

---

## Fault

Gate operation failed.

Controller SHALL determine recovery.

---

# Collector State Machine

The Collector Service owns the Collector lifecycle.

```text
Stopped

↓

Starting

↓

Running

↓

Run-On

↓

Stopping

↓

Stopped
```

---

## Stopped

Collector is not operating.

---

## Starting

StartCollector Command has been issued.

---

## Running

Collector reports operational.

---

## Run-On

No active Machine Units remain.

Run-on timer is active.

---

## Stopping

StopCollector Command has been issued.

---

# Route State Machine

The Routing Service owns Route activation.

```text
Inactive

↓

Active

↓

Inactive
```

---

## Inactive

No Machine Units require this Route.

---

## Active

At least one Machine Unit requires this Route.

The Route remains Active until all associated Machine Units have released extraction demand.

---

# Shared Gates

A Gate Unit MAY belong to multiple Routes.

The Routing Service SHALL maintain a reference count for every Gate Unit.

```text
Reference Count

0

↓

1

↓

2

↓

3

↓

2

↓

1

↓

0
```

The Gate Service SHALL:

Open gate when count transitions

```
0 → 1
```

Close gate when count transitions

```
1 → 0
```

Intermediate transitions SHALL NOT generate Commands.

---

# Manual Override

Manual Override SHALL temporarily suspend automatic behaviour.

Controller Services SHALL preserve internal state while Manual Override is active.

Automatic operation SHALL resume after Manual Override is released.

---

# Maintenance Mode

Maintenance Mode disables automatic operation.

Commands originating from automation SHALL NOT be generated.

Manual Commands remain permitted.

---

# Fault Handling

Faults SHALL be represented by Domain Events.

Controller Services SHALL determine appropriate recovery.

Examples include:

- GateFault
- CollectorFault
- MachineFault

Fault recovery SHALL be deterministic.

---

# Ownership

Every state SHALL have exactly one owner.

| State | Owner |
|--------|-------|
| Device lifecycle | Device Registry |
| Route activation | Routing Service |
| Gate state | Gate Service |
| Collector state | Collector Service |
| Configuration | Configuration Service |

No state SHALL have multiple owners.

---

# Design Principles

The state machine model SHALL satisfy the following principles.

- Every state has one owner.
- State changes occur only while processing Domain Events.
- State machines communicate through Events and Commands.
- State ownership SHALL remain explicit.
- Business logic SHALL remain deterministic.
- Hardware implementation SHALL remain independent of Controller logic.