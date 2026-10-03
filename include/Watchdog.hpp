#pragma once

#include "TaskScheduler.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

enum class WatchdogStatus
{
    Healthy,
    Fault
};

struct WatchdogAssessment
{
    WatchdogStatus status{WatchdogStatus::Healthy};

    std::vector<std::string> tasksWithDeadlineMisses;
};

class Watchdog
{
public:
    WatchdogAssessment evaluate(
        const std::vector<TaskStats>& taskStats);

    static const char* statusToString(
        WatchdogStatus status);

private:
    std::unordered_map<std::string, std::uint64_t>
        previousDeadlineMissCounts_;
};