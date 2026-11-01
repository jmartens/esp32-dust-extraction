# Current Transformer

`CurrentTransformer` implements the reusable `current_sensor::ICurrentSensor`
contract using the hardware-independent `adc::AdcSampler`. It calculates the
mean-removed RMS of a configured ADC sample window, converts ADC counts to
secondary voltage, then uses the burden resistance and CT turns ratio to
report primary RMS current.

Configure the primary-to-secondary turns ratio, burden resistance in ohms,
ADC volts per count, and sample count for the hardware profile. A CT without an
integrated burden requires an external burden resistor.

Calibration is runtime input through `apply_calibration`, using the
Controller-owned Machine Unit profile's `CalibrationState`. The sensor neither
owns nor persists that profile. An uncalibrated profile uses a unity factor.

The implementation and unit tests are hardware-independent. Physical
validation with the CT fixture and an ESP32 remains outstanding.
