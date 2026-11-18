#include "TelemetryLogger.hpp"

#include <cmath>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>

TelemetryLogger::TelemetryLogger(std::ostream& output)
    : output_(output)
{
    output_ << "timestamp,temperature_c,voltage_v,comms_ok\n";

    if (!output_)
    {
        throw std::runtime_error("Cannot write telemetry header.");
    }
}

void TelemetryLogger::writeSnapshot(
    const std::string& timestamp,
    const SystemState& state)
{
    if (!std::isfinite(state.temperatureC) ||
        !std::isfinite(state.voltageV))
    {
        throw std::invalid_argument(
            "Telemetry sensor values must be finite.");
    }

    // Timestamp formatting is the caller's responsibility.
    // Reject characters that could break the CSV structure.
    if (timestamp.empty() ||
        timestamp.find_first_of(",\"\r\n") != std::string::npos)
    {
        throw std::invalid_argument(
            "Invalid telemetry timestamp field.");
    }

    std::ostringstream row;
    row.imbue(std::locale::classic());

    row << timestamp << ','
        << std::setprecision(17)
        << state.temperatureC << ','
        << state.voltageV << ','
        << (state.communicationsHealthy ? "true" : "false")
        << '\n';

    output_ << row.str();

    if (!output_)
    {
        throw std::runtime_error("Cannot write telemetry snapshot.");
    }
}
