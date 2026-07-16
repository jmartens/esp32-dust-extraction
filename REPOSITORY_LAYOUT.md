# OpenExtraction Repository Layout & Development Strategy

## Overview

This document outlines the repository structure, coding standards, CI/CD pipeline, and layered architecture for OpenExtraction.

The repository follows strict architectural layering and configuration ownership principles as defined in `ARCHITECTURE.md`, `CONTRIBUTING.md`, and the Architecture Decision Records.

---

## Repository Structure

```
esp32-dust-extraction/
├── docs/
│   ├── 00-project-vision.md
│   ├── 01-system-architecture.md
│   ├── 02-design-principles.md
│   ├── 03-glossary.md
│   ├── 04-roadmap.md
│   ├── adr/                          # Architecture Decision Records
│   │   ├── 0001-architecture-first.md
│   │   ├── 0002-controller-authority.md
│   │   ├── 0003-immutable-device-identity.md
│   │   ├── 0004-routing-configuration-is-controller-owned.md
│   │   ├── 0005-use-rfc-2119-normative-language.md
│   │   ├── 0006-capabilities-over-device-types.md
│   │   ├── 0007-local-hardware-protection.md
│   │   ├── 0008-event-driven-controller-core.md
│   │   ├── 0009-separate-event-bus-and-command-bus.md
│   │   ├── 0010-event-based-service-communication.md
│   │   ├── 0011-transport-agnostic-business-logic.md
│   │   ├── 0012-controller-as-a-reactive-state-machine.md
│   │   ├── 0012-controller-owned-resource-namespace.md
│   │   └── 0013-domain-events-and-infrastructure-events.md
│   │
│   └── specification/                # Authoritative behavioral specs
│       ├── 00-introduction.md
│       ├── 01-terminology.md
│       ├── 02-system-requirements.md
│       └── ...
│
├── firmware/
│   ├── components/                   # ESP-IDF components (shared)
│   │   ├── domain/                   # Domain model (all firmware types)
│   │   │   ├── include/
│   │   │   │   ├── device_uid.hpp
│   │   │   │   ├── capabilities.hpp
│   │   │   │   ├── unit.hpp
│   │   │   │   ├── events.hpp
│   │   │   │   ├── commands.hpp
│   │   │   │   └── faults.hpp
│   │   │   └── src/
│   │   │       ├── device_uid.cpp
│   │   │       └── ...
│   │   │
│   │   ├── controller/               # Controller services (controller only)
│   │   │   ├── event_dispatcher/
│   │   │   │   ├── CMakeLists.txt
│   │   │   │   ├── include/
│   │   │   │   │   └── event_dispatcher.hpp
│   │   │   │   └── src/
│   │   │   │       └── event_dispatcher.cpp
│   │   │   ├── command_dispatcher/
│   │   │   ├── routing_service/
│   │   │   ├── configuration_service/
│   │   │   ├── device_registry/
│   │   │   ├── pairing_service/
│   │   │   └── diagnostics_service/
│   │   │
│   │   ├── runtime/                  # Generic unit runtime (all devices)
│   │   │   ├── device_runtime/
│   │   │   │   ├── include/
│   │   │   │   │   └── device_runtime.hpp
│   │   │   │   └── src/
│   │   │   │       └── device_runtime.cpp
│   │   │   └── units/
│   │   │       ├── machine_unit/
│   │   │       │   ├── include/
│   │   │       │   │   └── machine_unit.hpp
│   │   │       │   └── src/
│   │   │       │       └── machine_unit.cpp
│   │   │       ├── gate_unit/
│   │   │       │   ├── include/
│   │   │       │   │   └── gate_unit.hpp
│   │   │       │   └── src/
│   │   │       │       └── gate_unit.cpp
│   │   │       └── collector_unit/
│   │   │           ├── include/
│   │   │           │   └── collector_unit.hpp
│   │   │           └── src/
│   │   │               └── collector_unit.cpp
│   │   │
│   │   ├── transport/                # Transport layer (all devices)
│   │   │   ├── esp_now_transport/
│   │   │   │   ├── discovery_protocol/
│   │   │   │   ├── pairing_protocol/
│   │   │   │   ├── message_codec/
│   │   │   │   └── hostname/         # OE-{last-3-bytes-of-mac}
│   │   │   └── http_transport/
│   │   │
│   │   ├── persistence/
│   │   │   ├── nvs_storage/
│   │   │   └── pairing_store/
│   │   │
│   │   ├── hardware/                 # Low-level HAL
│   │   │   ├── mac_address_provider/
│   │   │   ├── relay_driver/
│   │   │   ├── sensor_driver/
│   │   │   ├── gpio_driver/
│   │   │   └── timer_driver/
│   │   │
│   │   └── testing/
│   │       ├── mocks/
│   │       │   ├── mock_transport.hpp
│   │       │   ├── mock_timer.hpp
│   │       │   ├── mock_storage.hpp
│   │       │   └── mock_gpio.hpp
│   │       └── fixtures/
│   │           └── test_factory.hpp
│   │
│   ├── binaries/                     # Separate firmware builds
│   │   ├── controller/               # Dedicated controller firmware
│   │   │   ├── CMakeLists.txt
│   │   │   ├── main.cpp
│   │   │   ├── Kconfig
│   │   │   └── app_controller.cpp
│   │   │
│   │   └── satellite/                # Dedicated satellite firmware
│   │       ├── CMakeLists.txt
│   │       ├── main.cpp
│   │       ├── Kconfig
│   │       └── app_satellite.cpp
│   │
│   ├── tests/
│   │   ├── unit/
│   │   │   ├── test_device_uid.cpp
│   │   │   ├── test_machine_unit.cpp
│   │   │   ├── test_gate_unit.cpp
│   │   │   ├── test_routing_service.cpp
│   │   │   ├── test_configuration_service.cpp
│   │   │   └── ...
│   │   ├── integration/
│   │   │   ├── test_discovery_and_pairing.cpp
│   │   │   ├── test_controller_flow.cpp
│   │   │   ├── test_satellite_with_configuration.cpp
│   │   │   └── ...
│   │   ├── CMakeLists.txt
│   │   └── conftest.py
│   │
│   ├── CMakeLists.txt                # Root CMake
│   ├── idf_component.yml
│   └── sdkconfig
│
├── ui/
│   ├── web/                          # Embedded web UI
│   │   ├── src/
│   │   ├── public/
│   │   └── package.json
│   └── docs/
│
├── tools/
│   ├── provisioning/
│   │   ├── workshop_config/
│   │   │   ├── workshop_setup_example.json
│   │   │   └── schema.json
│   │   ├── provision_controller.py
│   │   └── read_config.py
│   │
│   ├── scripts/
│   │   ├── bootstrap.sh              # Setup development environment
│   │   ├── format.sh                 # Run clang-format
│   │   ├── lint.sh                   # Run static analysis
│   │   └── test.sh                   # Run all test suites
│   │
│   ├── ci/
│   │   ├── build_controller.sh
│   │   ├── build_satellite.sh
│   │   └── test.sh
│   │
│   └── docker/
│       ├── Dockerfile.build
│       └── docker-compose.yml
│
├── .github/
│   ├── workflows/
│   │   ├── lint-and-format.yml
│   │   ├── build.yml                 # Build both controller & satellite
│   │   ├── test.yml                  # Unit + integration tests
│   │   ├── static-analysis.yml
│   │   ├── docs-validation.yml
│   │   └── release.yml
│   ├── CODEOWNERS
│   └── pull_request_template.md
│
├── .pre-commit-config.yaml
├── .gitignore
├── CMakeLists.txt
├── Makefile
├── ARCHITECTURE.md
├── CONTRIBUTING.md
├── COPILOT.md
├── README.md
├── REPOSITORY_LAYOUT.md               ← You are here
├── CHANGELOG.md
├── LICENSE
└── version.txt
```

---

## Device Identity and Naming Convention

### Three Distinct Concepts

1. **Device UID** (Domain Model)
   - Full MAC address: `84:F3:EB:12:A9:1C`
   - Generated at runtime from hardware
   - Immutable for device lifetime
   - Primary key for all resources and routing

2. **Hostname** (Transport Model)
   - Format: `OE-12A91C` (OpenExtraction prefix + last 3 bytes of MAC)
   - Used for mDNS discovery
   - Convenience logging/debugging
   - Transport concern only

3. **Friendly Name** (Metadata)
   - Set by controller during configuration
   - Examples: "table-saw", "router", "gate-1"
   - Optional display metadata
   - Never used as identifier

### Lifecycle

```
Device Powers On
    ↓
Generates UID from MAC: 84:F3:EB:12:A9:1C
Derives hostname: OE-12A91C
    ↓
Broadcasts discovery (UID + capabilities)
    ↓
Controller receives discovery
    ↓
Is UID already paired?
    ├─ NO: Check if UID in allowed list
    │   ├─ YES: Initiate pairing
    │   └─ NO: Reject
    │
    └─ YES: Send configuration
        └─ Device initializes with units
            └─ Operational

Controller may assign friendly name: "table-saw"
(shown in UI, logs; not used for routing/identification)
```

---

## Architectural Layering

### Domain Model
- **DeviceUID** (full MAC)
- **Events** (all UID-keyed)
- **Commands** (all UID-keyed)
- **Routes** (UID → UID)
- **Services** (routing, configuration, pairing)

**No transport concerns. No hostnames. No friendly names.**

### Information Model
- **Device Registry** (UID → capabilities, units)
- **Configuration** (UID → config, optional friendly_name)
- **Routes** (all UID-based)
- **Pairing Store** (which UIDs are authorized)

**Still no transport. Friendly names optional metadata.**

### Resource Model
- **Hierarchical namespace**: `/devices/{uid}`, `/machines/{uid}`, `/gates/{uid}`
- **Resources keyed by UID** (canonical)
- **Optional friendly_name** metadata for display
- **Hostname** available for reference

**Example**: `GET /devices/84:F3:EB:12:A9:1C` returns object with `uid`, `friendly_name`, `hostname`

### Interface Model (HTTP API / Web UI)
- **Primary queries by UID**: `GET /devices/{uid}`
- **Convenience filters**: `GET /devices?name=table-saw` (translates to UID internally)
- **Display both UID and friendly name** where available
- **All business logic converts friendly names to UIDs before operating**

### Transport Model
- **Hostname**: `OE-12A91C` (mDNS)
- **Discovery packets** contain full MAC (UID)
- **Pairing/configuration protocols** use UIDs
- **No business logic**

---

## Firmware Builds

### Two Dedicated Firmware Images

**Controller Firmware** (`binaries/controller/`)
- Runs only on controller ESP32
- Includes all controller services (routing, pairing, configuration)
- Can host local units (machine, gate, collector)
- Owns workshop configuration
- Manages device pairing and registration

**Satellite Firmware** (`binaries/satellite/`)
- Runs on all non-controller devices
- Minimal: only transport + unit runtime
- Discovers and pairs with controller
- Receives configuration from controller
- No business logic, no routing decisions

### Build Process

```bash
# Build controller firmware
idf.py -DFIRMWARE_TYPE=controller build

# Build satellite firmware
idf.py -DFIRMWARE_TYPE=satellite build

# Flash different devices
idf.py -p /dev/ttyUSB0 -DFIRMWARE_TYPE=controller flash
idf.py -p /dev/ttyUSB1 -DFIRMWARE_TYPE=satellite flash
```

---

## Configuration

### Controller Configuration (`tools/provisioning/workshop_config.json`)

**Single source of truth for workshop topology.** Stored on controller, distributed to satellites.

```json
{
  "allowed_devices": [
    {
      "uid": "84:F3:EB:12:A9:1C",
      "friendly_name": "table-saw",
      "units": [
        {
          "id": 0x01,
          "type": "Machine",
          "gpio_pin": 4,
          "sensor_type": "Current"
        },
        {
          "id": 0x02,
          "type": "Gate",
          "gpio_pin": 5,
          "actuator_type": "Relay"
        }
      ]
    }
  ],
  "routes": [
    {
      "demand_device": "84:F3:EB:12:A9:1C",
      "gate_devices": ["84:F3:EB:12:A9:1C"],
      "timeout_ms": 5000
    }
  ],
  "controller_local_units": [
    {
      "id": 0xFF01,
      "type": "Collector",
      "gpio_pin": 12
    }
  ]
}
```

**Key Principles:**
- All keys are UIDs, never friendly names
- Friendly names are optional metadata
- Routes reference UIDs only
- Controller loads this once at startup
- No device configuration files

---

## Coding Standards

### C++ (Modern C++20)

**Prefer:**
- `constexpr`, `enum class`, `std::optional`, `std::variant`, `std::span`, `std::string_view`
- RAII, composition over inheritance
- Dependency injection
- Explicit state machines
- Strong typing

**Avoid:**
- Macros, globals, singletons
- Raw owning pointers
- Magic numbers
- Premature optimization

### File Organization

```
components/my_service/
├── CMakeLists.txt
├── include/
│   ├── my_service.hpp          # Public interface
│   └── my_service_internal.hpp # Internal (not exported)
└── src/
    ├── my_service.cpp
    ├── helper.hpp              # Private types
    └── helper.cpp
```

### Naming Conventions

- **Classes/Types**: `PascalCase` (`RoutingService`, `DeviceUID`)
- **Functions/Methods**: `camelCase` (`startRoute()`, `registerDevice()`)
- **Constants**: `SCREAMING_SNAKE_CASE` (`MAX_ROUTES`, `DEFAULT_TIMEOUT_MS`)
- **Enums**: `enum class Name { Value }` (`enum class GateState { Open, Closed }`)
- **Files**: `snake_case.hpp/.cpp`

### Commit Messages (Conventional Commits)

```
feat(routing): implement route activation logic
fix(configuration): validate duplicate unit IDs
docs(specification): update resource model
refactor(event): simplify event dispatcher
test(pairing): add pairing protocol tests
ci(build): add satellite firmware workflow
```

---

## CI/CD Pipeline

### 1. Pre-Commit Checks (Local + Automated)
- Formatting (clang-format)
- Typo detection
- Large files, merge conflicts
- YAML validation
- CMake linting

### 2. Lint & Static Analysis
- clang-tidy
- cppcheck
- Style checks

### 3. Desktop Unit Tests
- Native C++ compilation
- No ESP-IDF required
- pytest for test fixtures
- Coverage reporting

### 4. ESP-IDF Build Tests
- Controller firmware build
- Satellite firmware build
- Binary artifacts uploaded

### 5. Integration Tests
- Device pairing workflows
- Configuration flows
- Routing scenarios

### 6. Documentation Validation
- ADR naming convention
- Specification completeness
- Link validation

### 7. Automatic Release
- semantic-release on merge to main
- Automatic versioning (SemVer)
- Changelog generation
- GitHub release creation

---

## Development Workflow

### Initial Setup

```bash
git clone https://github.com/jmartens/esp32-dust-extraction.git
cd esp32-dust-extraction

# Install dependencies, pre-commit hooks
./tools/scripts/bootstrap.sh

# Verify setup
make test
```

### Feature Development

```bash
# Create feature branch
git checkout -b feature/my-feature

# Make changes, commit with conventional message
git commit -m "feat(routing): implement route activation logic"

# Validate locally
make lint
make format
make test

# Push and create PR
git push origin feature/my-feature
```

### Branch Protection Rules

- ✓ Require PR reviews (1)
- ✓ Require CI status checks
- ✓ Require branches up-to-date
- ✓ Dismiss stale reviews
- ✓ Delete head branch on merge

### Release Process

**Automatic via semantic-release:**

1. PR merged to `main`
2. CI validates (all workflows pass)
3. semantic-release analyzes commits
4. Determines version bump (major/minor/patch)
5. Creates release tag
6. Generates changelog
7. Creates GitHub release

---

## Key Files

### Build & Configuration

- `CMakeLists.txt` — Root CMake configuration
- `idf_component.yml` — ESP-IDF component metadata
- `Makefile` — Convenience targets
- `.pre-commit-config.yaml` — Pre-commit hooks
- `.github/workflows/` — CI/CD workflows

### Documentation

- `ARCHITECTURE.md` — High-level overview
- `CONTRIBUTING.md` — Contribution guidelines
- `COPILOT.md` — Copilot instructions
- `REPOSITORY_LAYOUT.md` — This document
- `docs/adr/` — Architecture Decision Records
- `docs/specification/` — Behavioral specifications

### Development

- `tools/scripts/bootstrap.sh` — Environment setup
- `tools/scripts/test.sh` — Run tests
- `tools/scripts/lint.sh` — Run linters
- `tools/provisioning/workshop_config.json` — Configuration template

---

## Architecture Principles Summary

✅ **Architecture First**: Documentation → Design → Implementation  
✅ **Controller Authority**: Controller is sole decision maker  
✅ **Configuration Ownership**: Controller owns all configuration  
✅ **Immutable Identity**: Device UID (full MAC) never changes  
✅ **Event Driven**: All async interactions through events/commands  
✅ **Transport Independent**: Business logic decoupled from ESP-NOW/HTTP  
✅ **Hardware Independent**: Test without ESP32 hardware  
✅ **Layered Architecture**: Strict separation of domain/information/resource/interface/transport  
✅ **Friendly Names as Metadata**: UIDs are primary keys; names for display only  

---

## Next Steps

1. **Create CI workflows** (`.github/workflows/`)
2. **Create helper scripts** (`tools/scripts/`)
3. **Initialize component structure** with `CMakeLists.txt` templates
4. **Set up branch protection** on `main`
5. **Configure semantic-release** (`.releaserc.json`)
6. **Begin firmware implementation** following ADRs and specifications
