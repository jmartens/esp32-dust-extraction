# Current Transformer Test Fixture

This document defines a repeatable bench fixture for validating the
current-transformer (CT) machine-demand sensing chain (CT, burden resistor,
bias network and ESP32 ADC). It supports the Machine Demand Detection epic
(#39) and the CT calibration work.

**Status:** fixture design and procedure are documented. **No physical
measurements have been taken yet**; the results section is a pending template.

## Purpose

- Provide a repeatable way to inject known currents through a CT.
- Allow the ADC readings to be compared against a reference meter at several
  current levels.
- Produce data to derive calibration (scale, offset, noise floor, threshold)
  that is later supplied as Controller-owned Unit configuration (see
  ADR-0014 and `IMachineDemandSource`).

## Safety

Mains work can kill. Follow these rules without exception.

- The fixture MUST use a **low-voltage current-injection method** (below) in
  preference to switching mains loads, wherever it gives adequate accuracy.
- Any mains-powered part MUST be built by a competent person, be enclosed
  (no exposed live conductors), fused, and protected by an RCD/GFCI.
- Never open the CT secondary circuit while the primary carries current. Use a
  CT with a built-in burden (voltage-output type) or fit the burden resistor
  before energising.
- Do not clamp a CT around a conductor carrying more than the CT rating.
- Keep the ESP32 and USB connection isolated from mains; use a battery or
  isolated supply for the ESP32 when the primary is on mains.
- Use a TVS/clamp diode across the burden to protect the ADC input.
- Use only insulated, rated test leads and one hand at a time.
- Never leave the energised fixture unattended.

## Fixture Design

Two primary-current methods are supported. Method A is the default.

### Method A: multi-turn injection (low voltage, recommended)

A low-voltage AC source (for example a 12 V transformer or a signal generator
with a power amplifier) drives a series resistor/rheostat through a wire loop
that passes N times through the CT window. The CT sees N times the loop
current, so a small safe current simulates a larger one.

```text
 AC source --[R series]--[ammeter]--+
     |                              | N turns through CT window
     +------------------------------+
```

### Method B: mains resistive load (optional)

A known resistive load (for example a heater or lamp bank) switched in
steps, with the live conductor passing once through the CT. Use only with the
mains safety rules above.

### Sensing chain

```text
 CT secondary --+--[burden Rb]--+-- ADC input (via RC filter)
                |               |
               GND      bias divider to 3V3/2 (mid-rail), decoupling cap
```

- Bias the signal at mid-rail (about 1.65 V) so the AC waveform stays inside
  the ADC range.
- Choose Rb so that the peak voltage at the highest test current stays within
  the ADC range with margin. For a CT with turns ratio Nct and a maximum
  primary RMS current Imax: `Vpeak = Imax * sqrt(2) / Nct * Rb`, which SHOULD
  not exceed about 1.0 V (half-scale minus margin).

## Bill of Materials

Part values are indicative and MUST be confirmed against the CT datasheet.

| Item | Qty | Notes |
| --- | --- | --- |
| Split-core CT (e.g. SCT-013 style) | 1 | Record the exact model and ratio |
| Burden resistor Rb | 1 | 1 % metal film, value from formula above |
| Bias resistors 2 x 10 kOhm and 10 uF capacitor | 1 set | Mid-rail bias |
| RC low-pass filter (1 kOhm, 100 nF) | 1 | ADC anti-alias |
| TVS or Schottky clamp diodes | 2 | ADC protection |
| ESP32 dev board | 1 | ADC1 channel, e.g. GPIO34 |
| Isolated low-voltage AC source | 1 | Method A |
| Rheostat or resistor set | 1 | Sets current levels |
| Reference true-RMS multimeter or clamp meter | 1 | Calibrated reference |
| Enclosure, fuse, RCD | as needed | Method B only |

## Current Levels

Test at least five levels spanning the intended operating range, including
zero. The levels below are targets; record the **actual** reference reading.

| Level | Target RMS current (A, equivalent primary) |
| --- | --- |
| L0 | 0 (idle / noise floor) |
| L1 | 0.5 |
| L2 | 1 |
| L3 | 2 |
| L4 | 5 |
| L5 | 10 |

Equivalent primary current for Method A is `I_loop * N`.

## Test Procedure

1. Record environment: date, operator, firmware commit, CT model, Rb, supply
   voltage, temperature, mains frequency.
2. Inspect the wiring against the diagram and confirm the burden is fitted
   before energising.
3. Power the ESP32 and confirm the idle ADC mid-rail reading (offset) with
   the primary off.
4. For each level L0 to L5, in order:
   1. Set the current and wait 10 s to settle.
   2. Record the reference RMS current.
   3. Capture at least 10 s of raw ADC samples at a fixed sample rate
      (for example 1 kHz) with the ADC abstraction.
   4. Compute RMS of the AC component (mean removed) in ADC counts.
   5. Repeat the capture three times per level for repeatability.
5. Repeat the whole sweep in descending order to check hysteresis.
6. Return to L0 and confirm the noise floor is unchanged.
7. Record all values in the results table. Do not discard outliers; note
   them.

## Acceptance Criteria Mapping

| Issue #27 criterion | Where addressed |
| --- | --- |
| Supports repeatable measurements | Fixed fixture, repeated captures, up/down sweep |
| Documents test procedure | Test Procedure |
| Supports multiple current levels | Current Levels |
| Fixture documented | Fixture Design, BOM, Safety |
| Results recorded | Results (pending physical measurement) |

## Results

**PENDING.** No physical measurements have been recorded. Complete the
tables below after running the procedure on real hardware.

Run metadata:

| Field | Value |
| --- | --- |
| Date | _pending_ |
| Operator | _pending_ |
| Firmware commit | _pending_ |
| CT model / ratio | _pending_ |
| Burden Rb | _pending_ |
| Sample rate | _pending_ |

Measurements:

| Level | Reference I (A) | Run 1 RMS (counts) | Run 2 RMS (counts) | Run 3 RMS (counts) | Mean | Spread | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| L0 | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | |
| L1 | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | |
| L2 | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | |
| L3 | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | |
| L4 | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | |
| L5 | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | _pending_ | |

Derived values (from the data above): scale (A per count), offset, noise
floor, linearity error, repeatability. All _pending_.
