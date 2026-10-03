#include "FaultInjector.hpp"
#include "HealthMonitor.hpp"
#include "SystemState.hpp"
#include "TaskScheduler.hpp"
#include "Watchdog.hpp"

#include <thread>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

int main()
{
    std::cout << "RTOS System Health Monitor starting...\n";

    TaskScheduler scheduler;
    SystemState systemState;
    HealthMonitor healthMonitor;
    FaultInjector faultInjector;
    Watchdog watchdog;

    unsigned int elapsedSeconds = 0;
    unsigned int temperatureCycle = 0;
    unsigned int voltageCycle = 0;

    /*
     * Add the fault injector first so that when multiple tasks
     * become ready at the same instant, the injected fault state
     * is updated before the sensor-monitor tasks execute.
     */
    scheduler.addTask(
        "Fault Injector",
        1000ms,
        [&]()
        {
            ++elapsedSeconds;

            const FaultMode previousMode =
                faultInjector.mode();

            faultInjector.update(elapsedSeconds);

            if (faultInjector.mode() != previousMode)
            {
                std::cout
                    << "\n[FAULT INJECTION] Scenario: "
                    << FaultInjector::modeToString(
                           faultInjector.mode())
                    << "\n\n";
            }
        });

    scheduler.addTask(
        "Temperature Monitor",
        500ms,
        [&]()
        {
            ++temperatureCycle;

            if (faultInjector.overtemperatureActive())
            {
                systemState.temperatureC = 92.0;
            }
            else
            {
                systemState.temperatureC =
                    25.0 +
                    ((temperatureCycle % 6) * 0.5);
            }

            std::cout
                << "[TEMP] "
                << systemState.temperatureC
                << " C\n";
        });

    scheduler.addTask(
        "Voltage Monitor",
        250ms,
        [&]()
        {
            ++voltageCycle;

            if (faultInjector.undervoltageActive())
            {
                systemState.voltageV = 9.8;
            }
            else if ((voltageCycle % 2) == 0)
            {
                systemState.voltageV = 12.1;
            }
            else
            {
                systemState.voltageV = 11.9;
            }

            std::cout
                << "[VOLT] "
                << systemState.voltageV
                << " V\n";
        });

    scheduler.addTask(
        "Communications Monitor",
        1000ms,
        [&]()
        {
            systemState.communicationsHealthy =
                !faultInjector.communicationsFailureActive();

            std::cout
                << "[COMM] Link "
                << (systemState.communicationsHealthy
                        ? "OK"
                        : "FAULT")
                << '\n';
        });

    scheduler.addTask(
        "Health Supervisor",
        2000ms,
        [&]()
        {
            const HealthAssessment assessment =
                healthMonitor.evaluate(systemState);

            std::cout
                << "\n[HEALTH] Status: "
                << HealthMonitor::statusToString(
                       assessment.status)
                << " | Temp="
                << systemState.temperatureC
                << " C"
                << " | Voltage="
                << systemState.voltageV
                << " V"
                << " | Comms="
                << (systemState.communicationsHealthy
                        ? "OK"
                        : "FAULT")
                << " | Timing="
                << (systemState.timingHealthy
                        ? "OK"
                        : "FAULT")
                << "\n\n";
        });

    scheduler.addTask(
    "Diagnostics Task",
    500ms,
    [&]()
    {
        if (faultInjector.timingOverrunActive())
        {
            std::cout
                << "[DIAG] Simulating long-running task...\n";

            std::this_thread::sleep_for(700ms);
        }
        else
        {
            std::cout << "[DIAG] Diagnostics complete\n";
        }
    });

    scheduler.addTask(
    "Watchdog",
    1000ms,
    [&]()
    {
        const WatchdogAssessment assessment =
            watchdog.evaluate(
                scheduler.getTaskStats());

        systemState.timingHealthy =
            assessment.status ==
            WatchdogStatus::Healthy;

        std::cout
            << "\n[WATCHDOG] "
            << Watchdog::statusToString(
                   assessment.status);

        if (!assessment.tasksWithDeadlineMisses.empty())
        {
            std::cout << " | Deadline miss:";

            for (const auto& taskName :
                 assessment.tasksWithDeadlineMisses)
            {
                std::cout << " [" << taskName << "]";
            }
        }

        std::cout << "\n\n";
    });

    //gives the system enough time after the timing-overrun fault to visibly recover to NOMINAL
    std::cout
        << "Scheduler running for 20 seconds...\n\n";

    scheduler.runFor(20000ms);

    std::cout << "\nScheduler stopped.\n";

    return 0;
}