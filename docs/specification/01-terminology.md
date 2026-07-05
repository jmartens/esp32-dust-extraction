# docs/specification/01-terminology.md

# Terminology

This document defines terminology used throughout the project.

These definitions are normative.

## Controller

The Controller is the central decision-making node.

The Controller is responsible for:

- workshop configuration
- routing
- extraction scheduling
- gate coordination
- dust collector control
- security
- device registry

There SHALL be exactly one Controller in an OpenExtraction installation.

---

## Machine Node

A Machine Node is an embedded device attached to one woodworking machine.

Examples include:

- table saw
- planer
- jointer
- bandsaw
- router table

Machine Nodes detect extraction demand.

Machine Nodes MUST NOT control blast gates.

Machine Nodes MUST NOT start the dust collector.

---

## Gate Node

A Gate Node controls exactly one blast gate.

Gate Nodes execute commands issued by the Controller.

Gate Nodes MUST NOT make routing decisions.

---

## Device

A Device is any ESP participating in the OpenExtraction network.

Current device classes are:

- Controller
- Machine
- Gate

Future versions MAY introduce additional device classes.

---

## Physical Device

One ESP8266/ESP32.

Owns

MAC address
Device UID
pairing
firmware
transport

---

### Unit

A logical function hosted by a Physical Device.

Examples:

Machine
Gate
Collector
Sensor

Each Unit has:

Unit ID
Unit Type
Capabilities

---

### Capability

A capability describes behaviour.

Examples

DemandDetection

ManualOverride

Open

Close

PositionFeedback

CurrentMeasurement

TemperatureMeasurement

Capabilities describe what a Unit can do.

---

### Unit Type

Examples

Machine

Gate

Collector

EnvironmentalSensor

RelayOutput

AnalogInput

Unit Type identifies what it is.

Capability identifies what it can do.

---

## Device UID

Every Device possesses one immutable Device UID.

The Device UID identifies physical hardware.

The Device UID SHALL remain constant for the lifetime of the hardware.

Example:

MACHINE-7C9EBD002341

---

## Friendly Name

A Friendly Name is a human-readable label.

Examples:

Table Saw

Main Trunk Gate

Friendly Names are stored exclusively by the Controller.

Changing a Friendly Name SHALL NOT affect system behaviour.

---

## Route

A Route is the ordered collection of Gate UIDs required for extraction from a machine.

Routes are maintained by the Controller.

Routes SHALL reference immutable Gate UIDs.

---

## Extraction Demand

An Extraction Demand represents a request by a Machine Node for dust extraction.

Extraction Demand does not imply that extraction is immediately available.

The Controller determines when extraction begins.

---

## Capability

A Capability describes a function supported by a Device.

Capabilities are advertised by Devices during discovery.

Examples include:

- Position Feedback
- Timed Movement
- Current Measurement
- Auto Calibration

Capabilities determine behaviour.

Device models SHALL NOT determine behaviour.

---

## Configuration

Configuration represents workshop-specific information.

Examples include:

- Friendly Names
- Routes
- Timers
- Security Keys
- Device Assignments

Configuration is maintained exclusively by the Controller.
