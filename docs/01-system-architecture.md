# System Architecture

```mermaid
flowchart LR

    M1[Machine]
    M2[Machine]
    M3[Machine]

    G1[Gate]
    G2[Gate]
    G3[Gate]

    C[Controller]

    UI[Embedded Web UI]

    HA[Home Assistant]

    M1 --> C
    M2 --> C
    M3 --> C

    C --> G1
    C --> G2
    C --> G3

    UI --- C

    HA -. Optional .-> C
```

## Components

### Controller

Responsibilities:

- routing
- configuration
- security
- event logging
- collector control
- web interface

The controller is the only component that makes decisions.

### Machine Node

Responsibilities:

- detect machine operation
- request extraction
- manual override
- diagnostics

A machine node never starts the collector.

### Gate Node

Responsibilities:

- open gate
- close gate
- report position
- diagnostics

A gate node never decides whether it should move.

### Home Assistant

Optional.

Used for:

- dashboards
- monitoring
- notifications

Never participates in real-time control.