#pragma once

enum class FaultMode
{
    None,
    CommunicationsFailure,
    Undervoltage,
    Overtemperature,
    TimingOverrun
};

class FaultInjector
{
public:
    void update(unsigned int elapsedSeconds);

    FaultMode mode() const;

    bool communicationsFailureActive() const;
    bool undervoltageActive() const;
    bool overtemperatureActive() const;
    bool timingOverrunActive() const;

    static const char* modeToString(FaultMode mode);

private:
    FaultMode mode_{FaultMode::None};
};