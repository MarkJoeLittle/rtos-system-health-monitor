#pragma once

struct SystemState
{
    double temperatureC{25.0};
    double voltageV{12.0};

    bool communicationsHealthy{true};
    bool timingHealthy{true};
};