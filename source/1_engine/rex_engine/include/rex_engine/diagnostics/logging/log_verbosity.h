#pragma once

#include <rex_std/string.h>

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

  LogVerbosity operator&(int bits1, LogVerbosity bits2);
  LogVerbosity operator|(int bits1, LogVerbosity bits2);
  LogVerbosity operator&(LogVerbosity bits1, LogVerbosity bits2);
  LogVerbosity operator|(LogVerbosity bits1, LogVerbosity bits2);

  static_assert((int)LogVerbosity::NumVerbosity - 1 < (int)LogVerbosity::VerbosityMask, "Bad verbosity mask.");

  namespace Conversions
  {
    /**
     * Converts verbosity to a string
     * @param verbosity verbosity enum
     * @returns const char* representation of the verbosity enum
     */
    const char* toString(LogVerbosity verbosity);

    /**
     * Converts a string to verbosity
     * @param verbosity verbosity in string form (e.g., "Log", "Warning", or "Fatal")
     * @returns Parsed verbosity from the string (or Log if the string was invalid)
     */
    LogVerbosity parseLogVerbosityFromString(const rsl::string& verbosity);
  } // namespace Conversions
} // namespace rex
