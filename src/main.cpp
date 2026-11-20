#include "FaultInjector.hpp"
#include "HealthMonitor.hpp"
#include "SystemState.hpp"
#include "TaskScheduler.hpp"
#include "TelemetryLogger.hpp"
#include "Watchdog.hpp"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

using namespace std::chrono_literals;

namespace
{
struct CommandLineOptions
{
    bool telemetryEnabled{false};
    std::string telemetryPath;
    unsigned int durationSeconds{20};
    unsigned int telemetryIntervalMilliseconds{1000};
};

bool parsePositiveUnsigned(
    const std::string& value,
    unsigned int& parsedValue)
{
    if (value.empty() ||
        value.find_first_not_of("0123456789") != std::string::npos)
    {
        return false;
    }

    try
    {
        const unsigned long parsed = std::stoul(value);

        if (parsed == 0 ||
            parsed > std::numeric_limits<unsigned int>::max())
        {
            return false;
        }

        parsedValue = static_cast<unsigned int>(parsed);
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

bool parseCommandLine(
    int argc,
    char* argv[],
    CommandLineOptions& options)
{
    bool durationSpecified = false;
    bool telemetryIntervalSpecified = false;

    for (int index = 1; index < argc; ++index)
    {
        const std::string argument(argv[index]);

        if (argument == "--telemetry" &&
            !options.telemetryEnabled &&
            index + 1 < argc)
        {
            options.telemetryEnabled = true;
            options.telemetryPath = argv[++index];
        }
        else if (argument == "--duration" &&
                 !durationSpecified &&
                 index + 1 < argc)
        {
            durationSpecified = true;
            if (!parsePositiveUnsigned(
                    argv[++index],
                    options.durationSeconds))
            {
                return false;
            }
        }
        else if (argument == "--telemetry-interval-ms" &&
                 !telemetryIntervalSpecified &&
                 index + 1 < argc)
        {
            telemetryIntervalSpecified = true;
            if (!parsePositiveUnsigned(
                    argv[++index],
                    options.telemetryIntervalMilliseconds))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return !telemetryIntervalSpecified || options.telemetryEnabled;
}

void printUsage(const char* executable)
{
    std::cerr
        << "Usage: " << executable
        << " [--telemetry NEW_CSV_FILE]"
        << " [--duration SECONDS]"
        << " [--telemetry-interval-ms MILLISECONDS]\n";
}

std::string formatUtcTimestamp(
    std::chrono::system_clock::time_point timestamp)
{
    const auto wholeSeconds =
        std::chrono::time_point_cast<std::chrono::seconds>(timestamp);

    const auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(
            timestamp - wholeSeconds).count();

    const std::time_t rawTime =
        std::chrono::system_clock::to_time_t(wholeSeconds);

    const std::tm* utcPointer = std::gmtime(&rawTime);

    if (utcPointer == nullptr)
    {
        throw std::runtime_error("Cannot format telemetry timestamp.");
    }

    const std::tm utc = *utcPointer;

    std::ostringstream formatted;
    formatted.imbue(std::locale::classic());
    formatted << std::put_time(&utc, "%Y-%m-%dT%H:%M:%S")
              << '.'
              << std::setfill('0') << std::setw(6)
              << microseconds
              << 'Z';

    return formatted.str();
}
}

int main(int argc, char* argv[]) try
{
    std::ofstream telemetryFile;
    std::unique_ptr<TelemetryLogger> telemetryLogger;
    CommandLineOptions options;

    if (!parseCommandLine(argc, argv, options))
    {
        printUsage(argv[0]);
        return 1;
    }

    if (options.telemetryEnabled)
    {
        const std::filesystem::path destination(options.telemetryPath);

        if (std::filesystem::exists(destination))
        {
            throw std::runtime_error(
                "Telemetry destination already exists. Choose a new filename.");
        }

        telemetryFile.open(destination);

        if (!telemetryFile)
        {
            throw std::runtime_error(
                "Cannot open telemetry destination. Check its parent folder.");
        }

        telemetryLogger =
            std::make_unique<TelemetryLogger>(telemetryFile);
    }

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
    // Anchor calendar time once; measure subsequent elapsed time
    // with the steady clock so clock adjustments do not distort intervals.
    const auto telemetryEpoch = std::chrono::system_clock::now();
    const auto telemetryStart = TaskScheduler::Clock::now();

    if (telemetryLogger)
    {
        scheduler.addTask(
            "Telemetry Logger",
            std::chrono::milliseconds(
                options.telemetryIntervalMilliseconds),
            [&]()
            {
                const auto elapsed =
                    TaskScheduler::Clock::now() - telemetryStart;

                const auto timestamp =
                    telemetryEpoch +
                    std::chrono::duration_cast<
                        std::chrono::system_clock::duration>(elapsed);

                telemetryLogger->writeSnapshot(
                    formatUtcTimestamp(timestamp),
                    systemState);
            });
    }

    std::cout
        << "Scheduler running for "
        << options.durationSeconds
        << " seconds...\n\n";

    scheduler.runFor(
        std::chrono::seconds(options.durationSeconds));

    if (telemetryLogger)
    {
        telemetryFile.close();

        if (!telemetryFile)
        {
            throw std::runtime_error(
                "Cannot finish writing telemetry file.");
        }

        std::cout
            << "Telemetry saved to: "
            << options.telemetryPath
            << '\n';
    }

    std::cout << "\nScheduler stopped.\n";

    return 0;
}
catch (const std::exception& error)
{
    std::cerr << "Error: " << error.what() << '\n';
    return 1;
}
