# OpenExtraction

> Distributed ESP32-based dust extraction automation for woodworking shops.

## Overview

OpenExtraction is an open-source automation system for central dust extraction systems.

Unlike many hobby solutions, OpenExtraction is designed as a distributed embedded system rather than a collection of independent ESP sketches.

The project consists of:

- A central Controller node
- Machine nodes
- Blast gate nodes
- Optional sensor nodes
- An embedded web interface
- Optional Home Assistant integration

The system continues to operate when Wi-Fi or Home Assistant is unavailable.

## Design Goals

- Controller is the only decision maker.
- Real-time communication using ESP-NOW.
- No runtime dependency on Home Assistant.
- Modular architecture.
- Secure device pairing.
- Signed messages.
- Automatic device discovery.
- Simple hardware.
- Easy replacement of failed devices.
- Professional software architecture.

## Features (Planned)

- Automatic dust collector control
- Automatic blast gate control
- Manual machine overrides
- Manual collector override
- Configurable run-on timer
- Embedded web interface
- Device discovery
- Device pairing
- Secure communication
- OTA firmware updates
- Event logging
- Native unit tests

## Repository Structure

```
docs/
firmware/
ui/
config/
schemas/
tools/
```

## Status

Architecture and documentation are currently being developed.

Firmware implementation begins after the architecture specification has been completed.

## License

Apache 2.0