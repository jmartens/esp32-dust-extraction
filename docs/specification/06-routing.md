# Routing

## Purpose

This document defines how the Controller determines the extraction path for active Machine Units.

Routing is exclusively a Controller responsibility.

Physical Devices SHALL NOT perform routing decisions.

---

# Principles

The routing model is founded upon the following principles.

- Routes are owned by the Controller.
- Routes operate on Units.
- Routes are independent of hardware topology.
- Routes are deterministic.
- Multiple Routes MAY be active simultaneously.
- Shared Gate Units SHALL remain open while required.
- Physical Devices SHALL remain unaware of routing.

---

# Route

A Route is a directed graph of Units that defines the required extraction path for a Machine Unit.

A Route SHALL contain:

- one Machine Unit;
- one or more Gate Units.

Example:

```text
Machine Unit

↓

Table Saw

↓

Required Gates

- Main Gate
- Branch Gate
```

---

# Route Ownership

The Routing Service SHALL own all Routes.

The Routing Service SHALL determine:

- Route activation;
- Route deactivation;
- Gate reference counts;
- Route availability.

No other Service SHALL modify Routes.

---

# Route Activation

A Route SHALL become Active when its Machine Unit produces the Domain Event:

```
MachineDemandStarted
```

The Routing Service SHALL:

- activate the Route;
- increment the reference count of every required Gate Unit;
- emit:

```
RouteActivated
```

---

# Route Deactivation

A Route SHALL become Inactive after the corresponding Machine Unit produces:

```
MachineDemandStopped
```

The Routing Service SHALL:

- decrement the reference count of every required Gate Unit;
- emit:

```
RouteDeactivated
```

---

# Gate Reference Counting

Every Gate Unit SHALL maintain a logical reference count owned by the Routing Service.

Example:

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

Reference counts SHALL NOT be stored on Physical Devices.

---

# Gate Commands

The Gate Service SHALL issue Commands only when the reference count changes as follows:

Open Gate

```
0 → 1
```

Close Gate

```
1 → 0
```

Intermediate transitions SHALL NOT generate Commands.

Example:

```
1 → 2

No Command
```

---

# Shared Gates

A Gate Unit MAY belong to multiple Routes.

Example:

```text
                Main Gate
                     │
        ┌────────────┴────────────┐
        │                         │
Table Saw                  Thicknesser
```

The Main Gate SHALL remain open while at least one Route requires it.

---

# Multiple Active Routes

Multiple Machine Units MAY request extraction simultaneously.

The Routing Service SHALL activate every required Route.

Duplicate Gate Commands SHALL NOT be generated.

---

# Route Availability

The Routing Service SHALL determine whether a Route is available.

A Route SHALL be considered unavailable if:

- a required Gate Unit is unavailable;
- a required Unit reports a Fault;
- the Route is administratively disabled.

When a Route cannot be established, the Routing Service SHALL emit:

```
RequiredGateUnavailable
```

or another appropriate Domain Event.

The Collector SHALL NOT start until the route requirements defined by system policy have been satisfied.

---

# Hardware Independence

Routes SHALL reference Units.

Routes SHALL NOT reference:

- Physical Devices;
- MAC addresses;
- ESP-NOW peers;
- GPIO numbers.

Example:

Correct:

```text
Table Saw

↓

Main Gate
```

Incorrect:

```text
ESP32-1

↓

ESP32-3
```

---

# Dynamic Topology

The hosting Physical Device of a Unit MAY change without requiring Route modifications.

Only the Device Registry and Command Dispatcher SHALL be affected by such changes.

---

# Route Configuration

Route definitions are Configuration owned by the Configuration Service.

The Routing Service SHALL obtain Route definitions from the Configuration Service.

Route definitions SHALL be persistently stored.

The Routing Service SHALL NOT own Route definitions.

---

# Manual Override

Manual operation MAY temporarily bypass automatic routing.

The Routing Service SHALL preserve Route state while Manual Override is active.

Automatic routing SHALL resume when Manual Override is released.

---

# Route Lifecycle

```text
MachineDemandStarted

↓

Routing Service

↓

Activate Route

↓

Increment Gate Reference Counts

↓

RouteActivated

↓

Gate Service

↓

Generate OpenGate Commands

↓

Command Dispatcher
```

---

# Design Principles

The routing model SHALL satisfy the following principles.

- Routes belong to the Controller.
- Routes reference Units.
- Physical Devices remain transparent.
- Shared Gates use reference counting.
- Routing remains deterministic.
- Duplicate Commands are avoided.
- Multiple active Routes are supported.
- Hardware topology SHALL NOT affect routing.