# RTOS System Health Monitor

A C++17 embedded-systems portfolio project that simulates a real-time system health monitor with periodic task scheduling, fault injection, watchdog supervision, deadline-miss detection, automated verification, and DO-178C-inspired requirements traceability.

> **Note:** This project demonstrates software-engineering practices inspired by DO-178C (Software Considerations in Airborne Systems and Equipment Certification). It is an educational portfolio project and is not DO-178C certified.

---

## Project Overview

The RTOS System Health Monitor simulates a safety-oriented embedded system responsible for monitoring:

- System temperature
- Supply voltage
- Communications health
- Task execution timing
- Deadline violations

A cooperative scheduler executes periodic tasks at predefined intervals. Sensor-monitoring tasks update shared system state, while a health supervisor determines whether the system is operating in:

- `NOMINAL`
- `DEGRADED`
- `CRITICAL`

The project also includes deterministic fault injection so abnormal conditions can be reproduced and verified consistently.

---

## Key Features

- C++17 implementation
- CMake-based build system
- RTOS-style cooperative periodic scheduler
- Configurable task periods
- Shared system telemetry state
- Temperature monitoring
- Voltage monitoring
- Communications monitoring
- Health-state classification
- Deterministic fault injection
- Watchdog supervision
- Task execution-time measurement
- Deadline-miss detection
- Timing-fault propagation
- Fault recovery
- Automated unit testing with CTest
- Requirements-to-test traceability
- DO-178C-inspired development practices
- Windows Subsystem for Linux (WSL) development workflow

---

## System Architecture

```text
                         +------------------+
                         |  Fault Injector  |
                         +---------+--------+
                                   |
                                   v
+----------------------------------------------------------------+
|                    Periodic Task Scheduler                      |
|                                                                |
|  +-------------+ +-----------+ +----------------+ +-----------+|
|  | Temperature | |  Voltage  | | Communications | |Diagnostics||
|  |   Monitor   | |  Monitor  | |    Monitor     | |   Task    ||
|  +------+------+ +-----+-----+ +--------+-------+ +-----+-----+|
|         |              |                |               |       |
+---------|--------------|----------------|---------------|-------+
          |              |                |               |
          +--------------+----------------+---------------+
                                 |
                                 v
                         +---------------+
                         | System State  |
                         +-------+-------+
                                 |
                                 v
                         +---------------+
                         | Health Monitor|
                         +-------+-------+
                                 |
                    +------------+------------+
                    |            |            |
                 NOMINAL      DEGRADED     CRITICAL


Periodic Tasks
      |
      v
Execution-Time Statistics
      |
      v
Deadline-Miss Detection
      |
      v
Watchdog
      |
      v
Timing Health
      |
      v
Health Monitor