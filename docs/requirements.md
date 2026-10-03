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