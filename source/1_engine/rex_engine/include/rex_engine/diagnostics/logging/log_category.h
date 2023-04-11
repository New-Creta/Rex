#pragma once

#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/types.h"
#include "rex_std/string.h"

namespace rex
{
  using LogCategoryName = rex::DebugString;

  /** Base class for all log categories. **/
  struct LogCategoryBase
  {
    /**
     * Constructor, registers with the log suppression system and sets up the default values.
     * @param CategoryName, name of the category
     * @param InDefaultVerbosity, default verbosity for the category, may ignored and overridden by many other mechanisms
     **/
    LogCategoryBase(const LogCategoryName& inCategoryName, LogVerbosity inDefaultVerbosity);

    /** Gets the category name **/
    const LogCategoryName& get_category_name() const;

    /** Gets the working verbosity **/
    const LogVerbosity& get_verbosity() const;

    /** Should not generally be used directly. Tests the runtime verbosity and maybe triggers a debug break, etc. **/
    bool is_suppressed(LogVerbosity verbosityLevel) const;

    /** Sets up the working verbosity and clamps to the compile time verbosity. **/
    void set_verbosity(LogVerbosity verbosity);

  private:
    /** Holds the current suppression state **/
    LogVerbosity m_verbosity;

    /** Name for this category **/
    LogCategoryName m_category_name;
  };

  /** Template for log categories that transfers the compile-time constant default and compile time verbosity to the LogCategoryBase constructor. **/
  template <int TDefaultVerbosity>
  struct LogCategory : public LogCategoryBase
  {
    explicit LogCategory(const LogCategoryName& inCategoryName)
        : LogCategoryBase(inCategoryName, LogVerbosity(TDefaultVerbosity)) // NOLINT(google-readability-casting)
    {
      // assert((TDefaultVerbosity & LogVerbosity::VerbosityMask) < LogVerbosity::NumVerbosity && "Invalid default verbosity.");
    }
  };
} // namespace rex