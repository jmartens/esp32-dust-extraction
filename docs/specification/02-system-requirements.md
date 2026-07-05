# docs/specification/02-system-requirements.md

# System Requirements

This document defines the normative requirements for OpenExtraction.

All requirements in this document are mandatory unless explicitly stated otherwise.

---

# 1 Functional Requirements

## 1.1 Controller

### REQ-CTRL-001

The system SHALL contain exactly one Controller.

---

### REQ-CTRL-002

The Controller MUST maintain the complete workshop configuration.

The workshop configuration includes, but is not limited to:

- registered devices
- friendly names
- extraction routes
- timing parameters
- security configuration

---

### REQ-CTRL-003

The Controller MUST be the sole authority responsible for deciding:

- when the dust collector starts
- when the dust collector stops
- which gates open
- which gates close

---

### REQ-CTRL-004

The Controller MUST continue operating without Home Assistant.

---

### REQ-CTRL-005

The Controller MUST restore its persistent configuration after a restart.

---

### REQ-CTRL-006

The Controller MUST detect loss of communication with registered devices.

---

## 1.2 Machine Nodes

### REQ-MACH-001

A Machine Node MUST detect whether extraction is required.

The detection mechanism is implementation dependent.

Examples include:

- current transformer
- relay contact
- voltage detection
- digital input

---

### REQ-MACH-002

Machine Nodes MUST communicate extraction demand to the Controller.

---

### REQ-MACH-003

Machine Nodes MUST NOT contain routing information.

---

### REQ-MACH-004

Machine Nodes MUST NOT command Gate Nodes directly.

---

### REQ-MACH-005

Machine Nodes MUST support manual override.

The implementation SHALL provide:

- Automatic
- Force ON
- Force OFF

---

## 1.3 Gate Nodes

### REQ-GATE-001

Each Gate Node SHALL control exactly one blast gate.

---

### REQ-GATE-002

Gate Nodes MUST execute commands issued by the Controller.

---

### REQ-GATE-003

Gate Nodes MUST report movement completion when position feedback is available.

---

### REQ-GATE-004

Gate Nodes without position feedback MUST support configurable movement timing.

---

### REQ-GATE-005

Gate Nodes MUST advertise their capabilities during discovery.

---

## 1.4 Dust Collector

### REQ-DC-001

The dust collector SHALL start only after the Controller authorises startup.

---

### REQ-DC-002

The Controller SHOULD verify that required gates are open before starting the collector.

---

### REQ-DC-003

The maximum waiting time before collector startup SHALL be configurable.

---

### REQ-DC-004

After the final extraction demand disappears, the Controller SHALL maintain collector operation for a configurable run-on period.

---

### REQ-DC-005

The Controller SHALL stop the collector before commanding unnecessary gates to close.

---

# 2 Routing

### REQ-ROUTE-001

The Controller SHALL maintain the workshop routing configuration.

---

### REQ-ROUTE-002

Routes SHALL reference Gate Device UIDs.

---

### REQ-ROUTE-003

Machine Nodes SHALL NOT know their assigned route.

---

### REQ-ROUTE-004

Gate Nodes SHALL NOT know which machines use them.

---

### REQ-ROUTE-005

The Controller SHALL compute the union of all active routes.

---

### REQ-ROUTE-006

A route MAY contain zero gates.

An empty route indicates that no controllable blast gates are required.

---

# 3 Device Discovery

### REQ-DISC-001

Devices MUST automatically announce themselves after boot.

---

### REQ-DISC-002

The Controller MUST detect newly discovered devices.

---

### REQ-DISC-003

Unpaired devices MUST NOT participate in normal operation.

---

### REQ-DISC-004

The Controller SHALL support replacement of failed devices without firmware modification.

---

# 4 Security

### REQ-SEC-001

Every Device SHALL possess a unique immutable Device UID.

---

### REQ-SEC-002

Every Device SHALL possess a unique authentication key.

---

### REQ-SEC-003

Operational messages MUST be authenticated.

---

### REQ-SEC-004

Replay attacks MUST be detected.

---

### REQ-SEC-005

The Controller SHALL reject unauthenticated operational messages.

---

# 5 Persistence

### REQ-PERSIST-001

Device identity SHALL survive firmware updates.

---

### REQ-PERSIST-002

Workshop configuration SHALL survive Controller restart.

---

### REQ-PERSIST-003

Temporary communication loss SHALL NOT erase configuration.

---

# 6 Reliability

### REQ-REL-001

Loss of one Machine Node SHALL NOT affect unrelated machines.

---

### REQ-REL-002

Loss of one Gate Node SHALL NOT prevent communication with remaining Gate Nodes.

---

### REQ-REL-003

The Controller SHALL detect offline devices.

---

### REQ-REL-004

System recovery after temporary communication loss SHALL occur automatically.

---

# 7 Extensibility

### REQ-EXT-001

Protocol extensions SHALL be backward compatible whenever practical.

---

### REQ-EXT-002

Capabilities SHALL determine behaviour.

Hardware implementation SHALL NOT determine behaviour.

---

### REQ-EXT-003

Future device classes SHALL be introducible without redesigning the protocol.

# 8 Event driven

### REQ-EVT-001
All state transitions MUST be initiated by an event.

---

### REQ-EVT-002
Events MUST be immutable.

---

### REQ-EVT-003
Event processing MUST be sequential.

---

### REQ-EVT-004
Recursive event processing MUST NOT occur.

---

### REQ-EVT-005 
Events SHALL be processed in FIFO order unless explicitly prioritised.

---

### REQ-EVT-006
Business logic MUST NOT directly depend on transport implementations.

# 9 Commands
### REQ-CMD-001
Commands represent requests, not facts.

---

### REQ-CMD-002
Commands MUST NOT directly modify Controller state.

---

### REQ-CMD-003
Command completion SHALL be confirmed by subsequent events.

---

### REQ-CMD-004
Failed commands SHALL generate fault events.

# 9 Architecture
### REQ-ARCH-001
The Controller SHALL expose exactly one Event Bus.

---

### REQ-ARCH-002
The Controller SHALL expose exactly one Command Bus.

---

### REQ-ARCH-003
Transports SHALL translate packets to events and commands only.

---

### REQ-ARCH-004
Business logic SHALL NOT directly access transport layers.

---

### REQ-AECH-005
The systen SHALL support multiple logical units by a single Physical Device.

---

### REQ-ARCH-006
A Physical Device SHALL be able to expose Units of different Unit Types.


### REQ-ARCH-007
The system SHALL support multiple logical Units hosted by a single Physical Device.

### REQ-ARCH-008
A Physical Device SHALL be able to expose Units of different Unit Types.