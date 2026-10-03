#include "FaultInjector.hpp"

void FaultInjector::update(unsigned int elapsedSeconds)
{
    if (elapsedSeconds >= 3 && elapsedSeconds < 6)
    {
        mode_ = FaultMode::CommunicationsFailure;
    }
    else if (elapsedSeconds >= 6 && elapsedSeconds < 9)
    {
        mode_ = FaultMode::Undervoltage;
    }
    else if (elapsedSeconds >= 9 && elapsedSeconds < 12)
    {
        mode_ = FaultMode::Overtemperature;
    }
    else if (elapsedSeconds >= 12 && elapsedSeconds < 15)
    {
        mode_ = FaultMode::TimingOverrun;
    }
    else
    {
        mode_ = FaultMode::None;
    }
}

FaultMode FaultInjector::mode() const
{
    return mode_;
}

bool FaultInjector::communicationsFailureActive() const
{
    return mode_ == FaultMode::CommunicationsFailure;
}

bool FaultInjector::undervoltageActive() const
{
    return mode_ == FaultMode::Undervoltage;
}

bool FaultInjector::overtemperatureActive() const
{
    return mode_ == FaultMode::Overtemperature;
}

bool FaultInjector::timingOverrunActive() const
{
    return mode_ == FaultMode::TimingOverrun;
}

const char* FaultInjector::modeToString(FaultMode mode)
{
    switch (mode)
    {
        case FaultMode::None:
            return "NONE";

        case FaultMode::CommunicationsFailure:
            return "COMMUNICATIONS FAILURE";

        case FaultMode::Undervoltage:
            return "UNDERVOLTAGE";

        case FaultMode::Overtemperature:
            return "OVERTEMPERATURE";

        case FaultMode::TimingOverrun:
            return "TIMING OVERRUN";

        default:
            return "UNKNOWN";
    }
}