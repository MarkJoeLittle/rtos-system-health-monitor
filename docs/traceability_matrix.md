# Requirements Traceability Matrix

This matrix links software requirements to their implementation and automated
verification evidence.

| Requirement | Description | Implementation | Verification |
|---|---|---|---|
| HLR-HM-001 | Nominal conditions produce NOMINAL health | `src/HealthMonitor.cpp` | `test_health_monitor.cpp` — Nominal system |
| HLR-HM-002 | Communications failure produces DEGRADED health | `src/HealthMonitor.cpp` | `test_health_monitor.cpp` — Communications failure |
| HLR-HM-003 | Voltage below 10.5 V produces CRITICAL health | `src/HealthMonitor.cpp` | `test_health_monitor.cpp` — Undervoltage |
| HLR-HM-004 | Voltage above 13.5 V produces CRITICAL health | `src/HealthMonitor.cpp` | `test_health_monitor.cpp` — Overvoltage |
| HLR-HM-005 | Temperature above 85 C produces CRITICAL health | `src/HealthMonitor.cpp` | `test_health_monitor.cpp` — Overtemperature |
| HLR-HM-006 | Timing fault produces CRITICAL health | `src/HealthMonitor.cpp` | `test_health_monitor.cpp` — Timing failure |
| HLR-HM-007 | Critical fault overrides degraded fault | `src/HealthMonitor.cpp` | `test_health_monitor.cpp` — Critical fault overrides degraded fault |
| HLR-WD-001 | No new deadline misses produces HEALTHY | `src/Watchdog.cpp` | `test_watchdog.cpp` — No deadline misses |
| HLR-WD-002 | New deadline miss produces FAULT | `src/Watchdog.cpp` | `test_watchdog.cpp` — New deadline miss |
| HLR-WD-003 | Watchdog identifies faulted task | `src/Watchdog.cpp` | `test_watchdog.cpp` — Diagnostics/Voltage task identified |
| HLR-WD-004 | No subsequent miss permits recovery to HEALTHY | `src/Watchdog.cpp` | `test_watchdog.cpp` — No new deadline miss |
| HLR-SCH-001 | Periodic tasks wait for first release and execute subsequent callbacks | `src/TaskScheduler.cpp` | `test_task_scheduler.cpp` — Periodic task behavior |
| HLR-SCH-002 | Long callbacks increment deadline-miss counts | `src/TaskScheduler.cpp` | `test_task_scheduler.cpp` — Overrunning callback |
| HLR-SCH-003 | Scheduler rejects invalid task configurations | `src/TaskScheduler.cpp` | `test_task_scheduler.cpp` — Invalid task setup |
| HLR-SCH-004 | Simultaneous releases execute in registration order | `src/TaskScheduler.cpp` | `test_task_scheduler.cpp` — Coincident task releases |
| HLR-TLM-001 | Telemetry writes the documented, versioned schema | `src/TelemetryLogger.cpp`, `docs/telemetry_contract.md` | `test_telemetry_logger.cpp`, `test_telemetry_cli.py` |
| HLR-TLM-002 | Snapshots contain latest finite sensor values and communications health | `src/TelemetryLogger.cpp`, `src/main.cpp` | `test_telemetry_logger.cpp`, `test_telemetry_cli.py` |
| HLR-TLM-003 | Timestamps are UTC and strictly increasing | `src/main.cpp` | `test_telemetry_cli.py` |
| HLR-TLM-004 | Existing destinations are preserved and open errors are reported | `src/main.cpp` | `test_telemetry_cli.py` |
| HLR-CLI-001 | Capture duration and telemetry interval are configurable and validated | `src/main.cpp` | `test_telemetry_cli.py` |