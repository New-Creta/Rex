#pragma once

#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/types.h"

#include <rex_std/string.h>

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
    LogCategoryBase(const LogCategoryName& InCategoryName, LogVerbosity InDefaultVerbosity);

    /** Destructor, unregisters from the log suppression system **/
    ~LogCategoryBase();

    /** Gets the category name **/
    const LogCategoryName& getCategoryName() const;

    /** Gets the working verbosity **/
    const LogVerbosity& getVerbosity() const;

    /** Should not generally be used directly. Tests the runtime verbosity and maybe triggers a debug break, etc. **/
    bool isSuppressed(LogVerbosity VerbosityLevel) const;

    /** Sets up the working verbosity and clamps to the compile time verbosity. **/
    void setVerbosity(LogVerbosity Verbosity);

  private:
    /** Internal call to set up the working verbosity from the boot time default. **/
    void resetFromDefault();

    /** Holds the current suppression state **/
    LogVerbosity verbosity_;
    /** Holds default suppression **/
    LogVerbosity default_verbosity_;

    /** Name for this category **/
    const LogCategoryName category_name_;
  };

  /** Template for log categories that transfers the compile-time constant default and compile time verbosity to the LogCategoryBase constructor. **/
  template <int TDefaultVerbosity>
  struct LogCategory : public LogCategoryBase
  {
    LogCategory(const LogCategoryName& InCategoryName)
        : LogCategoryBase(InCategoryName, LogVerbosity(TDefaultVerbosity))
    {
      // assert((TDefaultVerbosity & LogVerbosity::VerbosityMask) < LogVerbosity::NumVerbosity && "Invalid default verbosity.");
    }
  };
} // namespace rex