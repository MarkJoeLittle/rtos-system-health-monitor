#include "HealthMonitor.hpp"
#include "SystemState.hpp"

#include <iostream>
#include <string>

int main()
{
    HealthMonitor monitor;
    int failures = 0;

    const auto verify =
        [&](const std::string& testName,
            const SystemState& state,
            HealthStatus expected)
        {
            const HealthAssessment result =
                monitor.evaluate(state);

            if (result.status != expected)
            {
                std::cerr
                    << "[FAIL] "
                    << testName
                    << " | Expected="
                    << HealthMonitor::statusToString(expected)
                    << " Actual="
                    << HealthMonitor::statusToString(result.status)
                    << '\n';

                ++failures;
            }
            else
            {
                std::cout
                    << "[PASS] "
                    << testName
                    << '\n';
            }
        };

    {
        SystemState state;

        verify(
            "HLR-HM-001: Nominal system",
            state,
            HealthStatus::Nominal);
    }

    {
        SystemState state;
        state.communicationsHealthy = false;

        verify(
            "HLR-HM-002: Communications failure",
            state,
            HealthStatus::Degraded);
    }

    {
        SystemState state;
        state.voltageV = 9.8;

        verify(
            "HLR-HM-003: Undervoltage",
            state,
            HealthStatus::Critical);
    }

    {
        SystemState state;
        state.voltageV = 14.0;

        verify(
            "HLR-HM-004: Overvoltage",
            state,
            HealthStatus::Critical);
    }

    {
        SystemState state;
        state.temperatureC = 92.0;

        verify(
            "HLR-HM-005: Overtemperature",
            state,
            HealthStatus::Critical);
    }

    {
        SystemState state;
        state.timingHealthy = false;

        verify(
            "HLR-HM-006: Timing failure",
            state,
            HealthStatus::Critical);
    }

    {
        SystemState state;
        state.communicationsHealthy = false;
        state.voltageV = 9.8;

        verify(
            "HLR-HM-007: Critical fault overrides degraded fault",
            state,
            HealthStatus::Critical);
    }

    if (failures == 0)
    {
        std::cout
            << "\nAll HealthMonitor tests passed.\n";

        return 0;
    }

    std::cerr
        << "\n"
        << failures
        << " HealthMonitor test(s) failed.\n";

    return 1;
}