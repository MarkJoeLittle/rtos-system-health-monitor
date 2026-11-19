# Software Requirements

## Purpose

This document defines the software requirements for the RTOS System Health
Monitor project.

The project demonstrates requirements-driven development practices inspired by
DO-178C (Software Considerations in Airborne Systems and Equipment
Certification).

This project is educational and is not certified to DO-178C.

---

## System Health Requirements

### HLR-HM-001 — Nominal Health State

When temperature, voltage, communications, and timing parameters are within
their acceptable operating limits, the software shall report the system health
state as NOMINAL.

Verification Method: Test

---

### HLR-HM-002 — Communications Fault

When the communications link is unhealthy and no critical fault is present, the
software shall report the system health state as DEGRADED.

Verification Method: Test

---

### HLR-HM-003 — Undervoltage Detection

When system voltage is below 10.5 volts, the software shall report the system
health state as CRITICAL.

Verification Method: Test

---

### HLR-HM-004 — Overvoltage Detection

When system voltage exceeds 13.5 volts, the software shall report the system
health state as CRITICAL.

Verification Method: Test

---

### HLR-HM-005 — Overtemperature Detection

When system temperature exceeds 85 degrees Celsius, the software shall report
the system health state as CRITICAL.

Verification Method: Test

---

### HLR-HM-006 — Timing Fault Detection

When the timing subsystem reports an unhealthy condition, the software shall
report the system health state as CRITICAL.

Verification Method: Test

---

### HLR-HM-007 — Fault Severity Precedence

When both a degraded-condition fault and a critical-condition fault are active,
the software shall report the system health state as CRITICAL.

Verification Method: Test

---

## Watchdog Requirements

### HLR-WD-001 — Healthy Timing State

When no new task deadline misses have occurred since the previous watchdog
evaluation, the watchdog shall report HEALTHY.

Verification Method: Test

---

### HLR-WD-002 — Deadline Miss Detection

When a task accumulates a new deadline miss, the watchdog shall report FAULT.

Verification Method: Test

---

### HLR-WD-003 — Faulted Task Identification

When a deadline miss is detected, the watchdog shall identify the task
associated with the new deadline miss.

Verification Method: Test

---

### HLR-WD-004 — Timing Recovery

After a previously detected deadline miss, if no additional deadline misses
occur before the next watchdog evaluation, the watchdog shall report HEALTHY.

Verification Method: Test

---

## Scheduler Requirements

### HLR-SCH-001 — Periodic Task Releases

When a task is registered with a positive period, the scheduler shall wait at
least one period before its first callback and shall execute callbacks on
subsequent releases while running.

Verification Method: Test

---

### HLR-SCH-002 — Missed Release Accounting

When a task callback delays execution beyond one or more scheduled releases,
the scheduler shall increment that task's deadline-miss count.

Verification Method: Test

---

### HLR-SCH-003 — Valid Task Configuration

The scheduler shall reject non-positive task periods and empty callbacks.

Verification Method: Test

---

### HLR-SCH-004 — Simultaneous Release Ordering

When multiple tasks are ready during the same scheduler iteration, the
scheduler shall execute them in registration order.

Verification Method: Test

---

## Telemetry Requirements

### HLR-TLM-001 — Versioned CSV Schema

When telemetry output is enabled, the software shall write the documented CSV
columns in the documented order.

Verification Method: Test

---

### HLR-TLM-002 — Snapshot Values

Each telemetry row shall contain the latest available finite temperature and
voltage values and the communications-health value when the telemetry task
runs.

Verification Method: Test

---

### HLR-TLM-003 — UTC Monotonic Timestamps

Each telemetry timestamp shall be formatted as UTC ISO 8601 with a trailing
`Z`. Timestamps shall represent elapsed time measured by a steady clock
anchored to UTC at startup and shall increase strictly between rows.

Verification Method: Test

---

### HLR-TLM-004 — Safe Output Destination

When telemetry output is enabled, the software shall not overwrite an existing
destination. If the destination cannot be opened, the software shall report
the error and exit unsuccessfully.

Verification Method: Test

---

### HLR-CLI-001 — Capture Configuration

The command line shall accept a positive whole-number capture duration in
seconds and, when telemetry is enabled, a positive whole-number telemetry
interval in milliseconds. Defaults shall be 20 seconds and 1000 milliseconds.
Invalid or incomplete options shall be rejected with usage information.

Verification Method: Test