#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

struct TaskStats
{
    std::string name;

    std::uint64_t executionCount{0};
    std::uint64_t deadlineMissCount{0};

    std::chrono::microseconds lastExecutionTime{0};
    std::chrono::microseconds maxExecutionTime{0};
};

class TaskScheduler
{
public:
    using Clock = std::chrono::steady_clock;
    using TaskCallback = std::function<void()>;

    void addTask(
        const std::string& name,
        std::chrono::milliseconds period,
        TaskCallback callback);

    void runFor(std::chrono::milliseconds duration);

    std::vector<TaskStats> getTaskStats() const;

private:
    struct Task
    {
        std::string name;
        std::chrono::milliseconds period;
        Clock::time_point nextRun;
        TaskCallback callback;
        TaskStats stats;
    };

    std::vector<Task> tasks_;
};