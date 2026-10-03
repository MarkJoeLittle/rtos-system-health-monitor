#pragma once

#include "SystemState.hpp"

enum class HealthStatus
{
    Nominal,
    Degraded,
    Critical
};

struct HealthAssessment
{
    HealthStatus status{HealthStatus::Nominal};

    bool temperatureFault{false};
    bool voltageFault{false};
    bool communicationsFault{false};
    bool timingFault{false};
};

class HealthMonitor
{
public:
    HealthAssessment evaluate(const SystemState& state) const;

    static const char* statusToString(HealthStatus status);
    

private:
    static constexpr double MAX_TEMPERATURE_C = 85.0;
    static constexpr double MIN_VOLTAGE_V = 10.5;
    static constexpr double MAX_VOLTAGE_V = 13.5;
};