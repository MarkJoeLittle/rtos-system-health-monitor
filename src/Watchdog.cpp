#include "Watchdog.hpp"

WatchdogAssessment Watchdog::evaluate(
    const std::vector<TaskStats>& taskStats)
{
    WatchdogAssessment assessment;

    for (const auto& stats : taskStats)
    {
        const std::uint64_t previousCount =
            previousDeadlineMissCounts_[stats.name];

        if (stats.deadlineMissCount > previousCount)
        {
            assessment.tasksWithDeadlineMisses.push_back(
                stats.name);
        }

        previousDeadlineMissCounts_[stats.name] =
            stats.deadlineMissCount;
    }

    if (!assessment.tasksWithDeadlineMisses.empty())
    {
        assessment.status = WatchdogStatus::Fault;
    }

    return assessment;
}

const char* Watchdog::statusToString(
    WatchdogStatus status)
{
    switch (status)
    {
        case WatchdogStatus::Healthy:
            return "HEALTHY";

        case WatchdogStatus::Fault:
            return "FAULT";

        default:
            return "UNKNOWN";
    }
}