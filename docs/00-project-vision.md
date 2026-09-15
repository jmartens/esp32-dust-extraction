# Project Vision

## Purpose

OpenExtraction aims to become a professional-quality distributed dust extraction controller for woodworking workshops.

The project emphasizes maintainability, robustness, and safety over simplicity.

## Motivation

Many DIY dust extraction systems consist of independent relay boards and ESP devices with ad hoc logic.

As more machines and blast gates are added, those systems become difficult to maintain and extend.

OpenExtraction treats the workshop as a distributed automation system.

## Scope

OpenExtraction covers the design and implementation of a distributed control
system for workshop dust extraction. The system coordinates machine demand,
blast gates, dust collectors, device identity, routing, configuration,
diagnostics, and the communication between those components.

The Controller owns operational decisions and workshop configuration. Device
firmware, transport adapters, hardware drivers, and user interfaces support
that controller-owned model.

## Non-goals

OpenExtraction does not aim to:

- replace a workshop's dust collection hardware;
- make safety decisions that belong to local electrical or mechanical protection systems;
- make autonomous routing decisions on machine or gate nodes; or
- prescribe one specific ESP32 board, relay, sensor, or workshop layout.

## Intended Audience

This project is intended for:

- workshop owners and makers building automated dust extraction systems;
- embedded firmware and hardware developers;
- contributors working on controller services, transports, and user   interfaces;
- maintainers reviewing architecture and implementation changes; and
- developers using AI-assisted coding tools within the project's documented  architectural constraints.

## Goals

### Functional

- Detect machine activity.
- Determine extraction demand.
- Open required blast gates.
- Start dust collector.
- Delay shutdown.
- Close unused gates.

### Non-functional

- Modular
- Secure
- Testable
- Extensible
- Offline capable
- Open source

## Success Criteria

The system shall:

- require no Home Assistant for normal operation
- tolerate Wi-Fi outages
- support any number of machines
- support any number of blast gates
- allow replacement of devices without rewriting firmware
- be configurable entirely from the controller

## Guiding Principle

Hardware should be replaceable.

Configuration should survive hardware replacement.

Identity should survive firmware updates.

The controller should always remain the single source of truth.
