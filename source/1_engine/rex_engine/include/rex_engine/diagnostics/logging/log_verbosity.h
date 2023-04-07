#pragma once

#include "rex_engine/types.h"

/**
 * Enum that defines the verbosity levels of the logging system.
 **/
namespace rex
{
  enum class LogVerbosity
  {
    /**
     * Not used
     */
    NoLogging = 0,

    /**
     * Always prints a fatal error to console (and log file) and crashes (even if logging is disabled)
     */
    Fatal,

    /**
     * Prints an error to console (and log file).
     */
    Error,

    /**
     * Prints a warning to console (and log file).
     */
    Warning,

    /**
     * Prints a message to the console (and log file).
     */
    Log,

    /**
     * Prints a verbose message to a log file
     * If Verbose logging is enabled for the given category, usually used for detailed logging.
     */
    Verbose,

    /**
     * Prints a verbose message to a log file
     * if VeryVerbose logging is enabled, usually used for detailed logging that would otherwise spam output.
     */
    VeryVerbose,

    // Log masks and special Enum values

    All = VeryVerbose,
    NumVerbosity,
    VerbosityMask = 0xf,
  };

  LogVerbosity operator&(unsigned int bits1, LogVerbosity bits2);
  LogVerbosity operator|(unsigned int bits1, LogVerbosity bits2);
  LogVerbosity operator&(LogVerbosity bits1, LogVerbosity bits2);
  LogVerbosity operator|(LogVerbosity bits1, LogVerbosity bits2);

  static_assert(static_cast<unsigned int>(LogVerbosity::NumVerbosity) - 1 < static_cast<unsigned int>(LogVerbosity::VerbosityMask), "Bad verbosity mask.");

  namespace conversions
  {
    /**
     * Converts verbosity to a string
     * @param verbosity verbosity enum
     * @returns const char* representation of the verbosity enum
     */
    const char* to_string(LogVerbosity verbosity);

    /**
     * Converts a string to verbosity
     * @param verbosity verbosity in string form (e.g., "Log", "Warning", or "Fatal")
     * @returns Parsed verbosity from the string (or Log if the string was invalid)
     */
    LogVerbosity parse_log_verbosity_from_string(const rex::DebugString& verbosity);
  } // namespace conversions
} // namespace rex
