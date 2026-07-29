# OpenExtraction Roadmap

## Purpose

This document describes the planned evolution of OpenExtraction.

The roadmap is intended to communicate implementation priorities and
provide contributors with a high-level understanding of the expected
delivery sequence.

The roadmap is intentionally iterative. Future milestones MAY be
adjusted based on workshop validation and operational experience.

---

## Guiding Principles

OpenExtraction prioritizes:

* Determinism
* Testability
* Transport independence
* Hardware independence
* Maintainability
* Extensibility

A working system is preferred over premature feature completeness.

---

## Milestone Strategy

The project follows an incremental approach:

1. Repository Bootstrap
2. First Dust
3. Automatic Extraction
4. Workshop MVP
5. Future Enhancements

Each milestone SHALL deliver a usable system.

---

## v0.0.1 - Repository Bootstrap

### Goal

Establish the repository, architecture and development practices.

### Deliverables

* Repository structure
* Documentation framework
* ADR framework
* GitHub templates
* CI/CD pipelines
* Coding standards
* CONTRIBUTING.md
* ARCHITECTURE.md
* COPILOT.md

### Result

A maintainable, contributor-friendly repository.

---

## v0.1.0 - First Dust

### Goal

Deliver the first operational prototype.

### Deliverables

#### Event System

* Event base class
* Event Dispatcher
* Command base class
* Command Dispatcher

#### Collector Service

* Collector State Machine
* StartCollector command
* StopCollector command
* CollectorStarted event
* CollectorStopped event

#### Collector Unit

* Collector Unit interface
* Relay abstraction
* ESP32 relay implementation

#### HTTP Interface

* Minimal HTTP server
* Collector API
* Minimal HTML interface

### Result

The dust collector can be started and stopped remotely.

---

## v0.2.0 - Automatic Extraction

### Goal

Provide automatic dust extraction.

### Deliverables

#### Machine Demand Detection

* IMachineDemandSource
* ESP32 ADC abstraction
* Current Transformer abstraction
* RMS current calculation
* Demand threshold evaluation
* Demand debounce support
* Current transformer calibration
* Current transformer test fixture
* MachineDemandStarted event
* MachineDemandStopped event

#### Collector Automation

* Collector run-on timer
* Collector start delay
* Demand aggregation
* Automatic collector control
* Manual override support
* Collector fault handling

### Result

The operator can switch on a machine and observe the dust collector
starting automatically.

---

## v0.3.0 - Workshop MVP

### Goal

Support multiple machines and establish the Workshop model.

### Planned Areas

* Workshop model
* Device model
* Unit model
* Capability model
* Multiple machine support
* Concurrent demand handling
* Safety mechanisms
* Watchdog support
* Fault propagation
* Diagnostics

### Result

OpenExtraction becomes suitable for daily workshop use.

---

## Future Milestones

Potential future milestones include:

* Blast gate support
* Route management
* OTA firmware updates
* Device discovery
* Pairing workflows
* Security improvements
* Web UI enhancements
* Diagnostics dashboard
* Historical telemetry
* Multi-controller support

Future milestones remain intentionally undefined.

---

## Roadmap Notes

Milestones v0.3.0 and beyond intentionally remain less detailed until
v0.1.0 and v0.2.0 have been validated in a real workshop.

Real-world experience is expected to influence future requirements,
particularly in the areas of:

* Machine profiles
* Fault handling
* Diagnostics
* Safety requirements
* Manual override semantics
* Workshop topology
* Routing behavior

---

## Definition of Success

OpenExtraction is considered successful when:

1. A machine can request extraction.
2. The Controller makes all operational decisions.
3. The collector operates automatically.
4. Multiple machines are supported.
5. The system remains deterministic and testable.
6. Workshop operation becomes more convenient and safer.
