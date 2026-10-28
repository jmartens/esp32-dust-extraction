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

The Collector Unit's commanded output is off. Without motor-feedback hardware,
this state does not confirm that the collector motor is physically stopped.
`CollectorStopped` reports that the output was commanded off; it SHALL NOT be
interpreted as confirmation that the motor stopped.

---

## Starting

StartCollector Command has been issued. `CollectorStarted` reports that the
Collector Unit's output was commanded on; without motor-feedback hardware, it
SHALL NOT be interpreted as confirmation that the motor started.

---

## Running

The Collector Unit's commanded output is on. Without motor-feedback hardware,
this state does not confirm that the collector motor is physically running.

---

## Run-On

No active Machine Units remain.

Run-on timer is active.

---

## Automatic Start Delay

In `AUTO` mode, a new aggregate demand SHALL begin a configurable start delay
before the Collector output is commanded on. The default delay SHALL be one
second; zero SHALL be valid. If demand ends before the delay expires, the
pending start SHALL be cancelled. Returning to `AUTO` with demand already
active SHALL command the output on immediately.

---

## Stopping

StopCollector Command has been issued.

---

## Collector Fault

A Collector fault SHALL be latched by the Collector Service. Automatic start
and stop commands SHALL NOT be retried while the fault is active. Recovery
requires an explicit operator reset. After reset, the Collector Service SHALL
return to Stopped and reevaluate current demand.

Without motor-feedback hardware, the Controller cannot detect a motor that
fails to start, stalls, or continues running after the output is commanded off.
These conditions SHALL NOT be reported as detected faults.

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

The Collector Service SHALL support `AUTO`, `FORCED_ON`, and `FORCED_OFF`
modes.

- `AUTO` applies automatic control to aggregated machine demand.
- `FORCED_ON` commands the Collector output on regardless of machine demand.
- `FORCED_OFF` commands the Collector output off and suppresses automatic
  starts, even while machine demand is active. The active demand SHALL remain
  observable so the operator can see that extraction is being suppressed.

When returning to `AUTO` with active demand, the Collector SHALL start
immediately; the configurable start delay applies to a new demand received
while already in `AUTO`, not to demand accumulated during an override. Normal
run-on behaviour applies after demand ends in `AUTO`.

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
