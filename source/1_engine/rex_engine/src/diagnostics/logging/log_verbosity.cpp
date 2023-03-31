#include "rex_engine/diagnostics/logging/log_verbosity.h"

namespace rex
{
    //-------------------------------------------------------------------------
    LogVerbosity operator&(int bits1, LogVerbosity bits2)
    {
        return (LogVerbosity)(bits1 & static_cast<int>(bits2));
    }

    //-------------------------------------------------------------------------
    LogVerbosity operator|(int bits1, LogVerbosity bits2)
    {
        return (LogVerbosity)(bits1 | static_cast<int>(bits2));
    }

    //-------------------------------------------------------------------------
    LogVerbosity operator&(LogVerbosity bits1, LogVerbosity bits2)
    {
        return (LogVerbosity)(static_cast<int>(bits1) & static_cast<int>(bits2));
    }

    //-------------------------------------------------------------------------
    LogVerbosity operator|(LogVerbosity bits1, LogVerbosity bits2)
    {
        return (LogVerbosity)(static_cast<int>(bits1) | static_cast<int>(bits2));
    }

    namespace Conversions
    {
        const char* toString(LogVerbosity verbosity)
        {
            switch (verbosity & LogVerbosity::VerbosityMask)
            {
            case LogVerbosity::NoLogging:
                return "NoLogging";
            case LogVerbosity::Fatal:
                return "Fatal";
            case LogVerbosity::Error:
                return "Error";
            case LogVerbosity::Warning:
                return "Warning";
            case LogVerbosity::Log:
                return "Log";
            case LogVerbosity::Verbose:
                return "Verbose";
            case LogVerbosity::VeryVerbose:
                return "VeryVerbose";
            }

            return "Unknown Verbosity";
        }

        LogVerbosity parseLogVerbosityFromString(const rsl::string& verbosity)
        {
            if (verbosity == "NoLogging")
            {
                return LogVerbosity::NoLogging;
            }
            else if (verbosity == "Fatal")
            {
                return LogVerbosity::Fatal;
            }
            else if (verbosity == "Error")
            {
                return LogVerbosity::Error;
            }
            else if (verbosity == "Warning")
            {
                return LogVerbosity::Warning;
            }
            else if (verbosity == "Log")
            {
                return LogVerbosity::Log;
            }
            else if (verbosity == "Verbose")
            {
                return LogVerbosity::Verbose;
            }
            else if (verbosity == "VeryVerbose")
            {
                return LogVerbosity::VeryVerbose;
            }

            // An unknown value is treated as log
            return LogVerbosity::Log;
        }
    }
}