# Telemetry CSV Contract

Contract version: 1

The C++ RTOS System Health Monitor writes UTF-8 CSV snapshots for analysis by
`telemetry-log-analyzer`.

## Header

Contract version 1 uses this exact header and column order:

```csv
timestamp,temperature_c,voltage_v,comms_ok
```

## Fields

- `timestamp`: UTC ISO 8601 timestamp ending in `Z` with six fractional
  digits, for example `2026-11-19T06:30:00.000000Z`.
- `temperature_c`: finite decimal Celsius value.
- `voltage_v`: finite decimal volt value.
- `comms_ok`: lowercase `true` or `false`.

Rows must contain a value for every column and have strictly increasing
timestamps. Each row is a periodic snapshot of the latest available values,
not an event record. UTC is anchored once at startup; elapsed time is measured
with a steady clock so wall-clock adjustments do not distort intervals.

The schema version is recorded in the analyzer runner's JSON manifest, not in
each CSV row.

The producer must not add, remove, reorder, or repurpose fields within
contract version 1. Any producer schema change must bump the contract version
and update producer and strict-consumer tests together.

## Analyzer input compatibility

The general-purpose analyzer remains permissive by default for backward
compatibility. It accepts reordered columns and additional named columns,
timestamps with UTC offsets or without timezone information, and
case-insensitive `true`/`false` values.

Pass `--strict-contract` to require producer conformance to contract version
1: the exact header and order above, UTC timestamps with six fractional
digits ending in `Z`, and lowercase `true` or `false`. Only strict-mode
reports identify `contract_version` as `1`; permissive reports set it to null.

## Analysis behavior

Sensor thresholds are part of the contract:

| Sensor | Fault condition |
|---|---|
| Temperature | greater than 85 degrees Celsius |
| Voltage | less than 10.5 volts |
| Voltage | greater than 13.5 volts |
| Communications | `comms_ok` is `false` |

The Python analyzer's `--expected-interval` is an analysis tolerance, not a
field in the CSV. The default logging interval is one second. Host scheduling
can add gaps; this demonstration does not imply a real-time guarantee.
