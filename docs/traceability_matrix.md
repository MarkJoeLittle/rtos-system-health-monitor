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