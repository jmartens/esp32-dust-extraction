# Controller Configuration Service

`ControllerConfigurationService` owns Machine Unit profiles and delegates
persistence to an injected `IMachineUnitProfileStorage`. Each profile contains
the Unit's `CalibrationState`; thresholds and debounce settings are outside this
component.

Profiles are validated before they are stored or applied. A missing profile is
reported as `not_found` and is never replaced with an implicit calibration.
During startup, load a Unit's profile and call
`apply_machine_unit_profile` with its demand source to apply the persisted
calibration. The demand source and sensing abstractions do not own persistence.

The service is hardware-independent. A platform storage implementation owns
the durable medium; tests use an in-memory fake implementing the same storage
interface.
