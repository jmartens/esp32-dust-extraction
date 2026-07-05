# Device Information

## Purpose

This document defines the information model used to describe Physical Devices and Units within the OpenExtraction architecture.

The information model is transport independent and SHALL be used consistently throughout the Controller, configuration model, Web API and communication protocols.

---

# Overview

The OpenExtraction architecture distinguishes between:

- Physical Devices
- Units
- Unit Types
- Capabilities
- State

Physical Devices host one or more Units.

Business logic operates exclusively upon Units.

---

# Physical Device

A Physical Device represents a single network participant.

A Physical Device SHALL:

- possess exactly one Device UID;
- participate in discovery;
- participate in pairing;
- communicate through a single transport endpoint;
- host one or more Units.

Examples include:

- ESP32
- ESP8266
- Future hardware platforms

---

# Physical Device Information

The Controller SHALL maintain the following information for every Physical Device.

Mandatory information:

- Device UID
- Hardware type
- Firmware version
- Online state
- Pairing state

Optional information:

- Friendly name
- Manufacturer
- Model information
- Serial number

---

# Unit

A Unit represents a logical function hosted by a Physical Device.

Each Unit SHALL:

- belong to exactly one Physical Device;
- possess exactly one Unit Identifier;
- possess exactly one Unit Type;
- advertise zero or more Capabilities;
- maintain operational State.

A Unit SHALL be independently addressable by the Controller.

---

# Unit Information

The Controller SHALL maintain the following information for every Unit.

Mandatory information:

- Unit Identifier
- Unit Type
- Capability list

Optional information:

- Friendly name
- Description
- Manufacturer-specific metadata

The Controller SHALL NOT assume any Unit Capabilities based solely upon Unit Type.

---

# Unit Types

Unit Types define the primary role of a Unit.

Examples include:

- Machine
- Gate
- Collector
- Sensor
- Relay Output
- Analog Input

The protocol SHALL permit additional Unit Types to be introduced without architectural changes.

---

# Capabilities

Capabilities describe behaviour supported by a Unit.

Capabilities SHALL:

- describe supported functionality;
- remain independent of Unit Type;
- be discoverable by the Controller.

Examples include:

Machine Unit

- Demand Detection
- Current Measurement
- Manual Override

Gate Unit

- Open
- Close
- Position Feedback

Collector Unit

- Start
- Stop
- Fault Reporting

---

# Unit State

State describes the current condition of a Unit.

Examples include:

Machine Unit

- demand active
- current value
- runtime

Gate Unit

- position
- moving
- fault state

Collector Unit

- running
- run-on active
- fault state

The exact State representation SHALL depend upon Unit Type and Capabilities.

---

# Discovery

Discovery SHALL occur at the Physical Device level.

A Physical Device SHALL advertise:

- Device UID
- Physical Device information
- hosted Units

Every hosted Unit SHALL advertise:

- Unit Identifier
- Unit Type
- Capabilities

The Controller SHALL construct its internal domain model using the discovered information.

---

# Unit Independence

Units hosted by the same Physical Device SHALL behave as independent logical entities.

Examples:

```text
ESP32

├── Machine Unit
│
├── Machine Unit
│
├── Gate Unit
│
└── Gate Unit
```

Each Unit SHALL:

- participate independently in routing;
- maintain independent state;
- expose independent capabilities.

---

# Hardware Independence

The Controller SHALL operate exclusively upon Units.

Business logic SHALL NOT depend upon:

- MAC addresses;
- GPIO assignments;
- transport-specific identifiers;
- hardware topology.

Physical Devices SHALL remain transparent to the business logic except where device-level operations are required.

---

# Friendly Names

Friendly names are Controller-managed metadata.

Examples include:

```text
DEVICE-4F1A

↓

Workshop South Controller


Unit 1

↓

Table Saw


Unit 2

↓

 Shaper


Unit 3

↓

Main Blast Gate
```

Friendly names SHALL NOT participate in business logic.

---

# Persistent Information

The Configuration Service SHALL persist:

Physical Device:

- pairing information
- friendly names
- metadata

Units:

- configuration
- friendly names
- calibration information

Operational state SHALL NOT necessarily be persisted unless required elsewhere in the specifications.

---

# Relationship Model

```text
Physical Device

1

↓

*

Unit

1

↓

1

Unit Type

1

↓

*

Capability


Unit

1

↓

*

State
```

Business logic operates upon Units.

Capabilities describe behaviour.

State describes the current condition of a Unit.

---

# Design Principles

The Device Information model SHALL satisfy the following principles.

- Physical Devices are transport participants.
- Units are logical entities.
- Capabilities describe behaviour.
- State describes current conditions.
- Business logic remains hardware independent.
- Discovery remains transport independent.
- Unit information is capability driven.
- Physical Devices may host multiple Units.