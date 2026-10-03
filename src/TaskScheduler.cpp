#include "TaskScheduler.hpp"

#include <algorithm>
#include <thread>
#include <utility>

void TaskScheduler::addTask(
    const std::string& name,
    std::chrono::milliseconds period,
    TaskCallback callback)
{
    Task task;

    task.name = name;
    task.period = period;
    task.nextRun = Clock::now() + period;
    task.callback = std::move(callback);
    task.stats.name = name;

    tasks_.push_back(std::move(task));
}

void TaskScheduler::runFor(std::chrono::milliseconds duration)
{
    const auto endTime = Clock::now() + duration;

    while (Clock::now() < endTime)
    {
        const auto now = Clock::now();

        for (auto& task : tasks_)
        {
            if (now >= task.nextRun)
            {
                /*
                 * If we are already more than one complete period late,
                 * count any task releases that could not be serviced.
                 */
                while ((task.nextRun + task.period) <= now)
                {
                    task.nextRun += task.period;
                    ++task.stats.deadlineMissCount;
                }

                const auto executionStart = Clock::now();

                task.callback();

                const auto executionEnd = Clock::now();

                const auto executionTime =
                    std::chrono::duration_cast<std::chrono::microseconds>(
                        executionEnd - executionStart);

                ++task.stats.executionCount;
                task.stats.lastExecutionTime = executionTime;

                task.stats.maxExecutionTime =
                    std::max(
                        task.stats.maxExecutionTime,
                        executionTime);

                /*
                 * Advance to the next expected release.
                 */
                task.nextRun += task.period;

                /*
                 * If callback execution ran past one or more future
                 * releases, those releases represent deadline misses.
                 */
                while (task.nextRun <= executionEnd)
                {
                    task.nextRun += task.period;
                    ++task.stats.deadlineMissCount;
                }
            }
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    }
}

std::vector<TaskStats> TaskScheduler::getTaskStats() const
{
    std::vector<TaskStats> stats;

    stats.reserve(tasks_.size());

    for (const auto& task : tasks_)
    {
        stats.push_back(task.stats);
    }

    return stats;
}