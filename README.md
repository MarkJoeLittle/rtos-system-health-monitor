# RTOS System Health Monitor

[![CI](https://github.com/MarkJoeLittle/rtos-system-health-monitor/actions/workflows/ci.yml/badge.svg)](https://github.com/MarkJoeLittle/rtos-system-health-monitor/actions/workflows/ci.yml)

A C++17 embedded-systems portfolio project that simulates a real-time system health monitor with periodic task scheduling, deterministic fault injection, watchdog supervision, deadline-miss detection, automated verification, and DO-178C-inspired requirements traceability.

> **Note:** This project demonstrates software-engineering practices inspired by DO-178C (Software Considerations in Airborne Systems and Equipment Certification). It is an educational portfolio project and is not DO-178C certified.

## Project Overview

The RTOS System Health Monitor simulates a safety-oriented embedded system responsible for monitoring:

- System temperature
- Supply voltage
- Communications health
- Task execution timing
- Deadline violations

A cooperative scheduler executes periodic tasks at predefined intervals. Sensor-monitoring tasks update shared system state, while a health supervisor classifies the system as `NOMINAL`, `DEGRADED`, or `CRITICAL`.

The project also includes deterministic fault injection so abnormal conditions can be reproduced and verified consistently.

## Key Features

- C++17 implementation
- CMake-based build system
- RTOS-style cooperative periodic scheduler
- Configurable task periods
- Shared telemetry state
- Temperature, voltage, and communications monitoring
- Deterministic fault injection
- Watchdog supervision
- Task execution-time measurement
- Deadline-miss detection
- Timing-fault propagation and recovery
- Automated verification with CTest
- GitHub Actions Continuous Integration (CI)
- Requirements-to-test traceability
- DO-178C-inspired development practices

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
| Temperature | Voltage | Communications | Diagnostics | Watchdog |
+------+-------------+---------------+-------------+--------------+
       |             |               |             |
       +-------------+---------------+-------------+
                             |
                             v
                      +--------------+
                      | System State |
                      +------+-------+
                             |
                             v
                      +--------------+
                      |Health Monitor|
                      +------+-------+
                             |
                  +----------+----------+
                  |          |          |
               NOMINAL    DEGRADED   CRITICAL

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
```

## Periodic Tasks

| Task | Period | Purpose |
|---|---:|---|
| Voltage Monitor | 250 ms | Monitors simulated supply voltage |
| Temperature Monitor | 500 ms | Monitors simulated temperature |
| Diagnostics Task | 500 ms | Simulates periodic diagnostic activity |
| Communications Monitor | 1000 ms | Monitors communications-link health |
| Fault Injector | 1000 ms | Activates deterministic fault scenarios |
| Watchdog | 1000 ms | Detects new task deadline misses |
| Health Supervisor | 2000 ms | Determines overall system health |

Because the scheduler is cooperative and single-threaded, a long-running task can delay other tasks and cause secondary deadline misses. This intentionally demonstrates timing interference in a real-time-style system.

## Health Classification

The system reports:

- `NOMINAL` when temperature, voltage, communications, and timing are healthy.
- `DEGRADED` when communications fail and no critical condition is active.
- `CRITICAL` when temperature or voltage exceed limits, or when timing health is lost.

Current critical thresholds:

- Temperature > 85 C
- Voltage < 10.5 V
- Voltage > 13.5 V
- Timing subsystem unhealthy

Critical conditions take precedence over degraded conditions.

## Deterministic Fault Injection

The demo progresses through a repeatable scenario:

```text
Normal operation
    -> Communications failure
    -> Undervoltage
    -> Overtemperature
    -> Timing overrun
    -> Recovery
```

Example injected conditions:

- Undervoltage: 9.8 V
- Overtemperature: 92 C
- Diagnostics execution time: about 700 ms for a 500 ms-period task

During the timing-overrun scenario, the watchdog can detect deadline misses in both the long-running Diagnostics Task and tasks delayed by it, such as the Voltage Monitor.

## Automated Verification

The project uses CTest for automated verification.

Current test executables:

- `test_health_monitor`
- `test_watchdog`

Health-monitor verification covers:

- `HLR-HM-001` Nominal conditions -> NOMINAL
- `HLR-HM-002` Communications failure -> DEGRADED
- `HLR-HM-003` Undervoltage -> CRITICAL
- `HLR-HM-004` Overvoltage -> CRITICAL
- `HLR-HM-005` Overtemperature -> CRITICAL
- `HLR-HM-006` Timing failure -> CRITICAL
- `HLR-HM-007` Critical fault overrides degraded fault

Watchdog verification covers:

- `HLR-WD-001` No deadline misses -> HEALTHY
- `HLR-WD-002` New deadline miss -> FAULT
- `HLR-WD-003` Faulted task is identified
- `HLR-WD-004` No additional deadline miss -> recovery to HEALTHY

Local verification result:

```text
100% tests passed, 0 tests failed out of 2
```

GitHub Actions also configures, builds, and runs the CTest suite automatically on pushes and pull requests to `main`.

## Requirements Traceability

The repository includes:

- `docs/requirements.md`
- `docs/traceability_matrix.md`

Example traceability chain:

```text
HLR-HM-003
    -> Requirement: voltage below 10.5 V shall produce CRITICAL health
    -> Implementation: src/HealthMonitor.cpp
    -> Verification: tests/test_health_monitor.cpp
```

This demonstrates a simplified requirements-driven development process used in high-assurance and safety-critical software environments.

## Repository Structure

```text
rtos-system-health-monitor/
|-- .github/workflows/ci.yml
|-- .gitignore
|-- CMakeLists.txt
|-- README.md
|-- docs/
|   |-- requirements.md
|   `-- traceability_matrix.md
|-- include/
|   |-- FaultInjector.hpp
|   |-- HealthMonitor.hpp
|   |-- SystemState.hpp
|   |-- TaskScheduler.hpp
|   `-- Watchdog.hpp
|-- src/
|   |-- FaultInjector.cpp
|   |-- HealthMonitor.cpp
|   |-- main.cpp
|   |-- TaskScheduler.cpp
|   `-- Watchdog.cpp
`-- tests/
    |-- test_health_monitor.cpp
    `-- test_watchdog.cpp
```

Generated build directories are excluded through `.gitignore`.

## Build and Run

### Linux / WSL

Configure:

```bash
cmake -S . -B build-wsl
```

Build:

```bash
cmake --build build-wsl
```

Run:

```bash
./build-wsl/rtos_health_monitor
```

Run all tests:

```bash
ctest --test-dir build-wsl --output-on-failure
```

Run individual verification executables:

```bash
./build-wsl/test_health_monitor
./build-wsl/test_watchdog
```

## Engineering Concepts Demonstrated

- Embedded C++
- Real-time scheduling concepts
- Periodic task execution
- Cooperative scheduling
- Deterministic fault injection
- Watchdog supervision
- Deadline monitoring
- Fault detection and propagation
- Health-state management
- Failure recovery
- Requirements-driven development
- Automated verification
- Requirements traceability
- CMake and CTest
- Continuous Integration (CI)
- Linux/WSL development
- Safety-critical software concepts

## DO-178C-Inspired Development

This project does not claim DO-178C certification or compliance. It demonstrates selected practices associated with high-assurance aerospace software development, including explicit software requirements, deterministic behavior, automated verification, requirement identifiers, requirements-to-test traceability, fault detection, and timing supervision.

## Future Enhancements

Potential extensions include:

- Scheduler-focused unit tests
- Task priorities
- Preemptive scheduling simulation
- Hardware-in-the-loop (HIL) simulation
- Sensor abstraction interfaces
- Controller Area Network (CAN) or serial-bus simulation
- Persistent event logging
- Code coverage reporting
- Static analysis
- Additional requirements coverage reports
- Hardware-target deployment

## License

This project is intended for educational and portfolio use.
