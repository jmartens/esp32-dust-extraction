# ADR-16: System Health, Initialization and Self-Test

**Status:** Accepted

## Context

Determining whether firmware has successfully booted is more complex than reaching `app_main()`.

An OpenExtraction device consists of multiple cooperating components, including:

* Services
* Hardware drivers
* Physical devices
* Logical units

Future releases are expected to introduce additional components.

The system also requires a uniform mechanism for:

* OTA boot verification
* Startup validation
* Diagnostics
* Health monitoring
* Controller status reporting

A single component failure should prevent the device from declaring itself operational.

## Decision

Every component participating in system startup SHALL expose a common health interface.

Examples include:

* Services
* Hardware drivers
* Physical devices
* Logical units

Each component SHALL participate in the system initialization sequence and report its health status.

The controller SHALL aggregate these individual health states into a single device health state.

A device SHALL only transition to **HEALTHY** when every mandatory component reports **HEALTHY**.

Only after reaching the **HEALTHY** state may the firmware confirm the OTA image.

## Component Health States

Every health provider SHALL expose one of the following states:

* UNKNOWN
* INITIALIZING
* HEALTHY
* DEGRADED
* FAILED
* STOPPING
* STOPPED

## Device Lifecycle

```
BOOTING
    │
    ▼
INITIALIZING
    │
    ▼
SELF TEST
    │
    ▼
HEALTHY
    │
    ├────────► OTA Confirmed
    │
    ▼
RUNNING
```

If a mandatory component fails:

```
BOOTING
    │
INITIALIZING
    │
SELF TEST
    │
FAILED
    │
REBOOT
    │
ESP-IDF Rollback
```

## Future Extensions

The controller MAY periodically request devices to perform additional self-tests.

Self-tests may be:

* Passive (status verification)
* Active (hardware exercise)
* Extended (maintenance diagnostics)

Results SHOULD be available through the Web API and Home Assistant integration.

## Consequences

### Advantages

* Uniform startup model.
* Robust OTA verification.
* Consistent diagnostics.
* Predictable initialization lifecycle.
* Unified health reporting across all components.
* Simple controller health aggregation.

### Disadvantages

* Slightly more framework code.
* Every mandatory component must participate in the lifecycle.
* Additional implementation effort for new component types.

## Related Issues

* Implement system health framework
* Implement component lifecycle interfaces
* Implement controller health aggregation
* Implement startup diagnostics
* Implement boot completion event
