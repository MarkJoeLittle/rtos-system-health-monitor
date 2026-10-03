#include "HealthMonitor.hpp"

HealthAssessment HealthMonitor::evaluate(
    const SystemState& state) const
{
    HealthAssessment assessment;

    assessment.temperatureFault =
        state.temperatureC > MAX_TEMPERATURE_C;

    assessment.voltageFault =
        state.voltageV < MIN_VOLTAGE_V ||
        state.voltageV > MAX_VOLTAGE_V;

    assessment.communicationsFault =
        !state.communicationsHealthy;

    assessment.timingFault =
        !state.timingHealthy;

    if (assessment.temperatureFault ||
        assessment.voltageFault ||
        assessment.timingFault)
    {
        assessment.status = HealthStatus::Critical;
    }
    else if (assessment.communicationsFault)
    {
        assessment.status = HealthStatus::Degraded;
    }
    else
    {
        assessment.status = HealthStatus::Nominal;
    }

    return assessment;
}

const char* HealthMonitor::statusToString(HealthStatus status)
{
    switch (status)
    {
        case HealthStatus::Nominal:
            return "NOMINAL";

        case HealthStatus::Degraded:
            return "DEGRADED";

        case HealthStatus::Critical:
            return "CRITICAL";

        default:
            return "UNKNOWN";
    }
}