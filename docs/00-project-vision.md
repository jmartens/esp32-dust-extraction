# Project Vision

## Purpose

OpenExtraction aims to become a professional-quality distributed dust extraction controller for woodworking workshops.

The project emphasizes maintainability, robustness, and safety over simplicity.

## Motivation

Many DIY dust extraction systems consist of independent relay boards and ESP devices with ad hoc logic.

As more machines and blast gates are added, those systems become difficult to maintain and extend.

OpenExtraction treats the workshop as a distributed automation system.

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