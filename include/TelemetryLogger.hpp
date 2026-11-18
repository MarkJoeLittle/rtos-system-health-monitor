#pragma once

#include "SystemState.hpp"

#include <ostream>
#include <string>

class TelemetryLogger
{
public:
    explicit TelemetryLogger(std::ostream& output);

    void writeSnapshot(
        const std::string& timestamp,
        const SystemState& state);

private:
    std::ostream& output_;
};
