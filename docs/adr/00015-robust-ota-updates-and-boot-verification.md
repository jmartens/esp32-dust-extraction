# ADR-15: Robust OTA Updates and Boot Verification

**Status:** Accepted

## Context

OpenExtraction devices are intended to be permanently deployed throughout a workshop and may become physically inaccessible after installation. Firmware updates must therefore be reliable and capable of recovering automatically from failures.

An interrupted or defective firmware update must never permanently brick a device.

ESP-IDF provides dual OTA partitions with automatic rollback support. The OpenExtraction architecture shall explicitly cooperate with this mechanism to ensure only successfully initialized firmware is accepted.

The controller shall also be able to determine whether a device has successfully completed its startup sequence after an OTA update.

## Decision

OpenExtraction SHALL use the ESP-IDF OTA framework with automatic rollback enabled.

A newly installed firmware SHALL NOT immediately mark itself as valid after boot.

Instead, firmware SHALL confirm the OTA image only after all of the following conditions are met:

* All required services have initialized successfully.
* All required hardware has initialized successfully.
* All mandatory self-tests have passed.
* The overall system health state is **HEALTHY**.
* No fatal initialization errors remain.

Only then SHALL the firmware invoke:

```cpp
esp_ota_mark_app_valid_cancel_rollback();
```

If any fatal initialization failure occurs before this point, the firmware SHALL reboot without confirming the new firmware image.

ESP-IDF SHALL automatically roll back to the previous firmware after repeated unsuccessful boot attempts.

## Rationale

Separating "firmware started" from "firmware operational" greatly improves update robustness.

Using ESP-IDF's built-in rollback mechanism avoids implementing a custom recovery solution while ensuring devices can recover automatically from defective firmware.

## Consequences

### Advantages

* Automatic recovery from faulty firmware.
* No manual intervention required after failed updates.
* Greatly reduces the risk of bricking devices.
* Leverages ESP-IDF's proven OTA implementation.
* Boot verification becomes deterministic.

### Disadvantages

* Firmware confirmation is delayed until startup completes.
* Initialization failures result in automatic rollback.
