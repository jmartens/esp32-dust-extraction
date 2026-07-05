# Message Protocol

## Purpose

The OpenExtraction Message Protocol defines all communication between Controller, Machine Nodes, Gate Nodes and future system components.

The protocol is designed to be:

- deterministic
- extensible
- transport independent
- compact
- authenticated
- versioned

The protocol SHALL remain independent of the underlying communication transport.

The current implementation uses ESP-NOW.

Future transports MAY include:

- Ethernet
- Wi-Fi
- MQTT
- CAN
- Serial

---

# Design Goals

The protocol SHALL:

- minimise bandwidth usage
- minimise packet size
- support authenticated communication
- support protocol versioning
- support future expansion
- support heterogeneous device types
- remain backwards compatible where practical

---

# Protocol Model

All communication SHALL use a common packet format.

Only the Payload differs between message types.

```

```
┌─────────────────────────────┐
│         Header              │
├─────────────────────────────┤
│         Payload             │
├─────────────────────────────┤
│        Signature            │
└─────────────────────────────┘
```

Every packet SHALL contain exactly one message.

---

# Protocol Version

The protocol SHALL define a Protocol Version.

Devices SHALL reject packets using unsupported protocol versions.

Version numbers SHALL follow semantic versioning.

Major versions MAY introduce breaking protocol changes.

Minor versions SHALL remain backwards compatible.

---

# Packet Header

Every packet SHALL begin with the following header.

| Field | Size | Description |
|--------|-----:|-------------|
| Magic | 16 bit | Packet identifier |
| Version | 16 bit | Protocol version |
| Message Type | 16 bit | Message identifier |
| Flags | 16 bit | Message flags |
| Sequence | 32 bit | Monotonic sequence number |
| Timestamp | 64 bit | Milliseconds since boot or epoch |
| Source UID | 64 bit | Sending device |
| Destination UID | 64 bit | Receiving device |
| Payload Length | 16 bit | Payload size in bytes |

All integer values SHALL use little-endian encoding.

---

# Magic Number

The Magic Number identifies OpenExtraction packets.

Value:

```
0x4F45
```

ASCII:

```
OE
```

Packets with invalid magic numbers SHALL be discarded immediately.

---

# Message Flags

Flags are bitfields.

| Bit | Meaning |
|----:|---------|
| 0 | Acknowledgement Required |
| 1 | Acknowledgement |
| 2 | Error |
| 3 | Broadcast |
| 4 | Signed |
| 5 | Reserved |
| 6 | Reserved |
| 7 | Reserved |
| 8-15 | Reserved |

Unknown flag bits SHALL be ignored.

---

# Addressing

Each packet contains:

- Source UID
- Destination UID

Device UIDs SHALL be immutable.

Examples:

```
CONTROLLER-AABBCCDDEEFF

MACHINE-84F3EB112233

GATE-7C9EBD445566
```

Broadcast packets SHALL use the broadcast UID.

```
FFFFFFFFFFFFFFFF
```

---

# Sequence Numbers

Every transmitting device SHALL maintain an independent sequence counter.

Sequence numbers SHALL increase monotonically.

Sequence numbers SHALL wrap after the maximum value.

Sequence numbers are used for:

- replay protection
- duplicate detection
- acknowledgement tracking

---

# Timestamp

Each packet SHALL include a timestamp.

The timestamp is primarily used for:

- diagnostics
- event correlation
- timeout detection

Clock synchronisation between devices is NOT required.

---

# Payload

The payload format depends upon the Message Type.

Payloads SHALL be encoded using fixed-width binary structures.

Variable-length payloads MAY be used where required.

Padding bytes SHALL be avoided.

---

# Signature

Authenticated packets SHALL include a cryptographic signature.

The signature SHALL cover:

- Header
- Payload

The Signature field itself SHALL NOT be included in the signature calculation.

Signature verification SHALL occur before the packet is processed.

Invalid signatures SHALL cause immediate packet rejection.

---

# Message Categories

Messages are divided into logical categories.

| Range | Category |
|------:|----------|
| 0x0000-0x00FF | Protocol |
| 0x0100-0x01FF | Discovery |
| 0x0200-0x02FF | Registration |
| 0x0300-0x03FF | Events |
| 0x0400-0x04FF | Commands |
| 0x0500-0x05FF | Configuration |
| 0x0600-0x06FF | Diagnostics |
| 0x0700-0x07FF | Security |
| 0x0800-0x08FF | Reserved |

Additional categories MAY be defined in future protocol revisions.

---

# Message Lifecycle

Incoming packets SHALL be processed in the following order.

```
Receive Packet
       │
       ▼
Validate Magic
       │
       ▼
Validate Version
       │
       ▼
Verify Signature
       │
       ▼
Validate Destination
       │
       ▼
Decode Payload
       │
       ▼
Translate to Event or Command
       │
       ▼
Publish to Controller
```

Packets failing any validation step SHALL be discarded.

Appropriate diagnostic Events MAY be generated.

---

# Transport Independence

The Message Protocol defines packet contents only.

Transport-specific concerns such as:

- ESP-NOW peer management
- retransmission
- radio channel
- Wi-Fi configuration

are outside the scope of this specification.

Transport Adapters SHALL translate protocol packets into the underlying transport representation.

Business logic SHALL remain unaware of transport implementation.

# Network Management

Network Management defines how devices discover one another, establish trust and maintain liveness.

---

# Device States

Every device SHALL operate in one of the following network states.

| State | Description |
|--------|-------------|
| Unpaired | Device has no trusted Controller |
| Pairing | Pairing procedure in progress |
| Registered | Trusted by a Controller |
| Online | Actively communicating |
| Offline | Communication timeout |

---

# Discovery

Discovery allows Controllers to detect nearby devices.

Discovery SHALL NOT require prior pairing.

Discovery SHALL use broadcast messages.

Discovery SHALL NOT modify persistent configuration.

---

## Discovery Request

Message Type

```
0x0100
```

Direction

```
Controller → Broadcast
```

Purpose

Requests all compatible devices to identify themselves.

Payload

| Field | Type |
|--------|------|
| Controller UID | uint64 |
| Protocol Version | uint16 |

---

## Discovery Response

Message Type

```
0x0101
```

Direction

```
Device → Controller
```

Purpose

Advertises a device.

Payload

| Field | Type |
|--------|------|
| Device UID | uint64 |
| Device Type | uint16 |
| Firmware Version | uint32 |
| Hardware Version | uint32 |
| Capability Flags | uint64 |
| Friendly Name | UTF-8 String (optional) |

The Controller SHALL NOT rely on the Friendly Name for identification.

---

# Registration

Registration adds a device to the Controller's device registry.

Registration SHALL occur only after successful pairing.

Registration SHALL NOT establish trust.

---

## Registration Request

Message Type

```
0x0200
```

Direction

```
Device → Controller
```

Payload

| Field | Type |
|--------|------|
| Device UID | uint64 |
| Nonce | uint64 |

---

## Registration Response

Message Type

```
0x0201
```

Payload

| Field | Type |
|--------|------|
| Accepted | bool |
| Assigned Friendly Name | string |
| Controller UID | uint64 |

Registration SHALL be idempotent.

Repeated Registration Requests SHALL NOT create duplicate devices.

---

# Pairing

Pairing establishes mutual trust.

Pairing SHALL require physical user interaction.

Examples include:

- Pair button
- Pair mode via Web UI
- Temporary pairing window

Unauthenticated remote pairing SHALL NOT be permitted.

---

## Pairing Request

Message Type

```
0x0700
```

Direction

```
Device → Controller
```

Payload

| Field | Type |
|--------|------|
| Device UID | uint64 |
| Public Key | Variable |
| Nonce | uint64 |

---

## Pairing Response

Message Type

```
0x0701
```

Payload

| Field | Type |
|--------|------|
| Accepted | bool |
| Controller Public Key | Variable |
| Session Nonce | uint64 |

Successful pairing SHALL establish:

- shared trust
- persistent identity
- signing keys

---

# Heartbeat

Heartbeats allow the Controller to determine whether devices remain online.

Heartbeat SHALL NOT modify configuration.

Heartbeat SHALL NOT imply extraction demand.

---

## Heartbeat Message

Message Type

```
0x0102
```

Direction

```
Device → Controller
```

Default Interval

```
30 seconds
```

Payload

| Field | Type |
|--------|------|
| Device UID | uint64 |
| Uptime | uint64 |
| Free Memory | uint32 |
| CPU Load | uint8 |
| Temperature | int16 |
| Fault Flags | uint32 |

---

# Heartbeat Response

Message Type

```
0x0103
```

Purpose

Acknowledges heartbeat.

Payload

None.

---

# Offline Detection

A device SHALL be considered Offline if:

```
Three consecutive Heartbeats are missed.
```

The Controller SHALL publish:

```
DeviceOffline
```

when a device transitions Offline.

When communication resumes, the Controller SHALL publish:

```
DeviceOnline
```

---

# Capability Advertisement

Capabilities describe behaviour.

Capabilities SHALL remain independent of hardware implementation.

Examples include:

```
SupportsCurrentMonitoring

SupportsPositionFeedback

SupportsCurrentFeedback

SupportsCalibration

SupportsRelayOutput

SupportsManualOverride

SupportsInhibitSwitch
```

Controller logic SHALL depend upon Capabilities.

Controller logic SHALL NOT depend upon implementation technology.

---

# Controller Restart

Following Controller restart:

1. Load persistent configuration.
2. Restore paired devices.
3. Begin Discovery.
4. Await Heartbeats.
5. Mark responding devices Online.
6. Publish DeviceOnline Events.

Devices SHALL NOT require re-pairing after Controller restart.

---

# Device Restart

Following device restart:

1. Restore persistent configuration.
2. Restore Controller trust.
3. Begin Heartbeats.
4. Re-register if necessary.

Device restart SHALL NOT require user intervention.

---

# Timeout Handling

If Registration fails:

- retry using exponential backoff.

If Pairing fails:

- remain Unpaired.

If Discovery fails:

- continue periodic Discovery.

---

# Design Principles

Network Management SHALL satisfy the following principles.

- Discovery is not Pairing.
- Registration is not Pairing.
- Pairing establishes trust.
- Registration establishes membership.
- Discovery establishes visibility.
- Device UIDs are immutable.
- Friendly Names are configuration.
- Capabilities define behaviour.
- Communication SHALL be authenticated after Pairing.

# Business Messages

Business Messages communicate Events and Commands between the Controller and Nodes.

Business Messages SHALL use one of two protocol message types:

| Message Type | Identifier |
|--------------|-----------:|
| Event Message | 0x0300 |
| Command Message | 0x0400 |

The specific Event or Command SHALL be identified by an identifier contained within the payload.

---

# Event Messages

An Event Message communicates a fact.

Event Messages SHALL be immutable.

Event Messages SHALL describe something that has already occurred.

Event Messages SHALL NOT request another device to perform an action.

---

## Event Message Payload

| Field | Type | Description |
|--------|------|-------------|
| Event ID | uint16 | Event identifier |
| Event Version | uint8 | Payload version |
| Payload | Variable | Event-specific data |

---

# Event Identifiers

The Controller defines the Event namespace.

Event identifiers SHALL remain stable across protocol revisions.

Identifiers SHALL be grouped like below:

Event types | Address range 
|---:|---|
Machine Events | 0x1000-0x10FF
Gate Events | 0x1100-0x11FF
Collector Events | 0x1200-0x12FF
Controller Events | 0x1300-0x13FF
Gate Commands | 0x2100-0x21FF
Collector Commands | 0x2200-0x22FF
Controller Commands | 0x2300-0x23FF

| Event ID | Name |
|----------:|------|
| 0x0001 | MachineDemandStarted |
| 0x0002 | MachineDemandStopped |
| 0x0003 | GateOpening |
| 0x0004 | GateOpened |
| 0x0005 | GateClosing |
| 0x0006 | GateClosed |
| 0x0007 | GateFault |
| 0x0008 | CollectorStarted |
| 0x0009 | CollectorStopped |
| 0x000A | ManualOverrideChanged |
| 0x000B | ConfigurationChanged |
| 0x000C | DiagnosticsUpdated |

Additional Event identifiers MAY be defined in future protocol revisions.

---

# Event Payloads

Every Event defines its own payload.

Example:

MachineDemandStarted

| Field | Type |
|--------|------|
| Machine UID | uint64 |
| Demand Source | uint8 |
| Timestamp | uint64 |

Example:

GateOpened

| Field | Type |
|--------|------|
| Gate UID | uint64 |
| Position | uint16 |

Example:

GateFault

| Field | Type |
|--------|------|
| Gate UID | uint64 |
| Fault Code | uint16 |

---

# Command Messages

Commands request another device to perform an action.

Commands SHALL NOT directly modify Controller state.

Completion SHALL be confirmed by one or more subsequent Events.

---

## Command Payload

| Field | Type |
|--------|------|
| Command ID | uint16 |
| Command Version | uint8 |
| Payload | Variable |

---

# Command Identifiers

| Command ID | Name |
|-----------:|------|
| 0x0001 | OpenGate |
| 0x0002 | CloseGate |
| 0x0003 | StartCollector |
| 0x0004 | StopCollector |
| 0x0005 | RequestStatus |
| 0x0006 | RestartDevice |
| 0x0007 | BeginCalibration |
| 0x0008 | EndCalibration |
| 0x0009 | FactoryReset |

---

# Command Payload Examples

OpenGate

| Field | Type |
|--------|------|
| Gate UID | uint64 |

CloseGate

| Field | Type |
|--------|------|
| Gate UID | uint64 |

RequestStatus

No payload.

RestartDevice

| Field | Type |
|--------|------|
| Delay | uint16 |

---

# Acknowledgements

Successful receipt of a Command MAY be acknowledged.

Acknowledgement SHALL indicate only that the Command was received.

Acknowledgement SHALL NOT indicate successful completion.

Successful execution SHALL always be reported using one or more subsequent Events.

Example:

```
OpenGate (Command)

↓

ACK

↓

GateOpening (Event)

↓

GateOpened (Event)
```

---

# Duplicate Commands

Nodes SHALL detect duplicate Commands using the Sequence Number.

Duplicate Commands SHALL NOT be executed multiple times.

A duplicate Command MAY be acknowledged again.

---

# Unknown Commands

Unknown Commands SHALL be rejected.

The receiving device SHOULD generate a Diagnostic Event describing the failure.

---

# Unknown Events

Unknown Events SHALL be ignored.

Reception of an unknown Event SHALL NOT affect device behaviour.

Unknown Events MAY generate diagnostic information.

---

# Event Ordering

Events originating from a single device SHALL preserve transmission order.

Events originating from different devices are not globally ordered.

The Controller SHALL process Events in the order in which they are received.

---

# Delivery Guarantees

The protocol provides at-most-once delivery.

The application SHALL tolerate occasional packet loss.

State convergence SHALL be achieved through subsequent Events and Heartbeats rather than repeated transmission of historical Events.

---

# Design Principles

Business Messages SHALL satisfy the following principles.

- Events describe facts.
- Commands request behaviour.
- Commands never imply success.
- Events are immutable.
- Event identifiers remain stable.
- Payloads are versioned.
- Duplicate Commands SHALL be ignored.
- Business logic remains independent of transport.

