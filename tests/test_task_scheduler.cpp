#include "TaskScheduler.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

template <typename Callback>
bool throwsInvalidArgument(Callback callback)
{
    try
    {
        callback();
    }
    catch (const std::invalid_argument&)
    {
        return true;
    }
    catch (...)
    {
        return false;
    }

    return false;
}

int main()
{
    int failures = 0;
    const auto verify = [&](bool condition, const std::string& name)
    {
        std::cout << (condition ? "[PASS] " : "[FAIL] ")
                  << name << '\n';

        if (!condition)
        {
            ++failures;
        }
    };

    {
        TaskScheduler scheduler;
        std::vector<TaskScheduler::Clock::time_point> executions;
        const auto start = TaskScheduler::Clock::now();

        scheduler.addTask(
            "Periodic",
            5ms,
            [&]()
            {
                executions.push_back(TaskScheduler::Clock::now());
            });

        scheduler.runFor(100ms);

        const auto stats = scheduler.getTaskStats();
        verify(
            executions.size() >= 3 &&
                executions.size() == stats.front().executionCount,
            "Periodic task executes and updates its execution count");
        verify(
            !executions.empty() &&
                executions.front() - start >= 5ms,
            "Periodic task waits for its first release");
    }

    {
        TaskScheduler scheduler;
        std::vector<std::string> executionOrder;

        scheduler.addTask(
            "First",
            20ms,
            [&]()
            {
                executionOrder.push_back("first");
            });
        scheduler.addTask(
            "Second",
            20ms,
            [&]()
            {
                executionOrder.push_back("second");
            });

        scheduler.runFor(50ms);

        verify(
            executionOrder.size() >= 2 &&
                executionOrder[0] == "first" &&
                executionOrder[1] == "second",
            "Tasks execute in registration order when releases coincide");
    }

    {
        TaskScheduler scheduler;
        scheduler.addTask(
            "Overrunning",
            3ms,
            []()
            {
                std::this_thread::sleep_for(8ms);
            });

        scheduler.runFor(30ms);

        const auto stats = scheduler.getTaskStats();
        verify(
            stats.front().deadlineMissCount > 0,
            "Long callback execution records missed releases");
    }

    {
        TaskScheduler scheduler;
        verify(
            throwsInvalidArgument([&]
            {
                scheduler.addTask("Zero period", 0ms, [] {});
            }),
            "Reject zero task period");
        verify(
            throwsInvalidArgument([&]
            {
                scheduler.addTask("Negative period", -1ms, [] {});
            }),
            "Reject negative task period");
        verify(
            throwsInvalidArgument([&]
            {
                scheduler.addTask(
                    "Empty callback",
                    1ms,
                    TaskScheduler::TaskCallback{});
            }),
            "Reject empty task callback");
    }

    return failures == 0 ? 0 : 1;
}
