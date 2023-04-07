#include "rex_engine/diagnostics/logging/log_verbosity.h"

namespace rex
{
  //-------------------------------------------------------------------------
  LogVerbosity operator&(unsigned int bits1, LogVerbosity bits2)
  {
    return static_cast<LogVerbosity>(bits1 & static_cast<unsigned int>(bits2));
  }

  //-------------------------------------------------------------------------
  LogVerbosity operator|(unsigned int bits1, LogVerbosity bits2)
  {
    return static_cast<LogVerbosity>(bits1 | static_cast<unsigned int>(bits2));
  }

  //-------------------------------------------------------------------------
  LogVerbosity operator&(LogVerbosity bits1, LogVerbosity bits2)
  {
    return static_cast<LogVerbosity>(static_cast<unsigned int>(bits1) & static_cast<unsigned int>(bits2));
  }

  //-------------------------------------------------------------------------
  LogVerbosity operator|(LogVerbosity bits1, LogVerbosity bits2)
  {
    return static_cast<LogVerbosity>(static_cast<unsigned int>(bits1) | static_cast<unsigned int>(bits2));
  }

  namespace conversions
  {
    const char* to_string(LogVerbosity verbosity)
    {
      switch(verbosity & LogVerbosity::VerbosityMask)
      {
        case LogVerbosity::NoLogging: return "NoLogging";
        case LogVerbosity::Fatal: return "Fatal";
        case LogVerbosity::Error: return "Error";
        case LogVerbosity::Warning: return "Warning";
        case LogVerbosity::Log: return "Log";
        case LogVerbosity::Verbose: return "Verbose";
        case LogVerbosity::VeryVerbose: return "VeryVerbose";
        default: break;
      }

      return "Unknown Verbosity";
    }

    LogVerbosity parse_log_verbosity_from_string(const rex::DebugString& verbosity)
    {
      if(verbosity == "NoLogging")
      {
        return LogVerbosity::NoLogging;
      }
      else if(verbosity == "Fatal")
      {
        return LogVerbosity::Fatal;
      }
      else if(verbosity == "Error")
      {
        return LogVerbosity::Error;
      }
      else if(verbosity == "Warning")
      {
        return LogVerbosity::Warning;
      }
      else if(verbosity == "Log")
      {
        return LogVerbosity::Log;
      }
      else if(verbosity == "Verbose")
      {
        return LogVerbosity::Verbose;
      }
      else if(verbosity == "VeryVerbose")
      {
        return LogVerbosity::VeryVerbose;
      }

      // An unknown value is treated as log
      return LogVerbosity::Log;
    }
  } // namespace conversions
} // namespace rex