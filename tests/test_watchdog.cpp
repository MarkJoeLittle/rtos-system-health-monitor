#include "Watchdog.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

bool containsTask(
    const std::vector<std::string>& tasks,
    const std::string& name)
{
    return std::find(
               tasks.begin(),
               tasks.end(),
               name)
        != tasks.end();
}

int main()
{
    Watchdog watchdog;
    int failures = 0;

    auto verify =
        [&](bool condition,
            const std::string& testName)
        {
            if (condition)
            {
                std::cout
                    << "[PASS] "
                    << testName
                    << '\n';
            }
            else
            {
                std::cerr
                    << "[FAIL] "
                    << testName
                    << '\n';

                ++failures;
            }
        };

    std::vector<TaskStats> stats{
        TaskStats{"Voltage Monitor"},
        TaskStats{"Diagnostics Task"}
    };

    // No deadline misses.
    {
        const WatchdogAssessment result =
            watchdog.evaluate(stats);

        verify(
            result.status == WatchdogStatus::Healthy,
            "HLR-WD-001: No deadline misses -> HEALTHY");
    }

    // Introduce the first timing fault.
    stats[1].deadlineMissCount = 1;

    {
        const WatchdogAssessment result =
            watchdog.evaluate(stats);

        verify(
            result.status == WatchdogStatus::Fault,
            "HLR-WD-002: New deadline miss -> FAULT");

        verify(
            containsTask(
                result.tasksWithDeadlineMisses,
                "Diagnostics Task"),
            "HLR-WD-003: Diagnostics task identified");
    }

    // Same historical count should not create a new fault.
    {
        const WatchdogAssessment result =
            watchdog.evaluate(stats);

        verify(
            result.status == WatchdogStatus::Healthy,
            "HLR-WD-004: No new deadline miss -> HEALTHY");
    }

    // Introduce another deadline miss.
    stats[1].deadlineMissCount = 2;

    {
        const WatchdogAssessment result =
            watchdog.evaluate(stats);

        verify(
            result.status == WatchdogStatus::Fault,
            "HLR-WD-002: Additional deadline miss -> FAULT");
    }

    // Simulate another task missing its deadline.
    stats[0].deadlineMissCount = 1;

    {
        const WatchdogAssessment result =
            watchdog.evaluate(stats);

        verify(
            result.status == WatchdogStatus::Fault,
            "HLR-WD-002: Voltage deadline miss -> FAULT");

        verify(
            containsTask(
                result.tasksWithDeadlineMisses,
                "Voltage Monitor"),
            "HLR-WD-003: Voltage task identified");
    }

    if (failures == 0)
    {
        std::cout
            << "\nAll Watchdog tests passed.\n";

        return 0;
    }

    std::cerr
        << "\n"
        << failures
        << " Watchdog test(s) failed.\n";

    return 1;
}