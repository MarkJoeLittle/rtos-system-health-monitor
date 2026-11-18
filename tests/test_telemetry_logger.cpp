#include "TelemetryLogger.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

template <typename Exception, typename Callback>
bool throwsExpected(Callback callback)
{
    try
    {
        callback();
    }
    catch (const Exception&)
    {
        return true;
    }
    catch (...)
    {
        return false;
    }

    return false;
}

int main()
{
    int failures = 0;

    const auto verify = [&](bool condition, const std::string& name)
    {
        std::cout << (condition ? "[PASS] " : "[FAIL] ")
                  << name << '\n';

        if (!condition)
        {
            ++failures;
        }
    };

    const std::string header =
        "timestamp,temperature_c,voltage_v,comms_ok\n";

    {
        std::ostringstream output;
        TelemetryLogger logger(output);
        SystemState state;

        verify(output.str() == header, "Exact CSV header");

        logger.writeSnapshot("2026-10-22T10:00:00Z", state);

        state.temperatureC = 92.0;
        state.voltageV = 9.5;
        state.communicationsHealthy = false;
        logger.writeSnapshot("2026-10-22T10:00:01Z", state);

        verify(
            output.str() ==
                header +
                "2026-10-22T10:00:00Z,25,12,true\n"
                "2026-10-22T10:00:01Z,92,9.5,false\n",
            "Nominal and fault snapshots append correctly");
    }

    {
        std::ostringstream output;
        TelemetryLogger logger(output);
        SystemState state;

        state.temperatureC = std::numeric_limits<double>::quiet_NaN();

        verify(
            throwsExpected<std::invalid_argument>([&]
            {
                logger.writeSnapshot("2026-10-22T10:00:00Z", state);
            }),
            "Reject nonfinite temperature");

        state.temperatureC = 25.0;
        state.voltageV = std::numeric_limits<double>::infinity();

        verify(
            throwsExpected<std::invalid_argument>([&]
            {
                logger.writeSnapshot("2026-10-22T10:00:00Z", state);
            }),
            "Reject nonfinite voltage");

        verify(output.str() == header, "Invalid readings append no rows");
    }

    {
        std::ostringstream output;
        TelemetryLogger logger(output);
        SystemState state;

        for (const std::string timestamp :
             {"", "bad,value", "bad\"value", "bad\nvalue", "bad\rvalue"})
        {
            verify(
                throwsExpected<std::invalid_argument>([&]
                {
                    logger.writeSnapshot(timestamp, state);
                }),
                "Reject empty or CSV-breaking timestamp");
        }

        verify(output.str() == header, "Invalid timestamps append no rows");
    }

    {
        std::ostringstream output;
        output.setstate(std::ios::badbit);

        verify(
            throwsExpected<std::runtime_error>([&]
            {
                TelemetryLogger logger(output);
            }),
            "Report header write failure");
    }

    {
        std::ostringstream output;
        TelemetryLogger logger(output);
        output.setstate(std::ios::badbit);

        verify(
            throwsExpected<std::runtime_error>([&]
            {
                logger.writeSnapshot(
                    "2026-10-22T10:00:00Z", SystemState{});
            }),
            "Report snapshot write failure");
    }

    return failures == 0 ? 0 : 1;
}
