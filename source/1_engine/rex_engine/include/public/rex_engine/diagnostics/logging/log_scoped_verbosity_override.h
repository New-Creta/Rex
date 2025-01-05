#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"

namespace rex
{
  struct LogCategory;

  /*-----------------------------------------------------------------------------
          LogScopedVerbosityOverride
  -----------------------------------------------------------------------------*/
  /**
   * Helper class that allows setting scoped verbosity for log category.
   * Saved what was previous verbosity for the category, and recovers it when it goes out of scope.
   * Use Macro LOG_SCOPE_VERBOSITY_OVERRIDE for this
   **/
  class LogScopedVerbosityOverride
  {
  public:
    /** Back up the existing verbosity for the category then sets new verbosity.*/
    LogScopedVerbosityOverride(LogCategory* category, log::LogVerbosity verbosity);

    /** Restore the verbosity overrides for the category to the previous value.*/
    ~LogScopedVerbosityOverride();

    // Disable accidental copies
    LogScopedVerbosityOverride(const LogScopedVerbosityOverride&)            = delete;
    LogScopedVerbosityOverride& operator=(const LogScopedVerbosityOverride&) = delete;

  private:
    /** Backup of the category, verbosity pairs that was present when we were constructed **/
    LogCategory* m_saved_category;
    log::level::LevelEnum m_saved_log_level;
  };

  /**
   * A macro to override Verbosity of the Category within the scope
   * @param CategoryName, category to declare
   * @param ScopeVerbosity, verbosity to override
   **/
} // namespace rex

#if TB_NO_LOGGING
  #define TB_LOG_SCOPE_VERBOSITY_OVERRIDE(...)
#else
  #define TB_LOG_SCOPE_VERBOSITY_OVERRIDE(CategoryName, ScopeVerbosity) rex::LogScopedVerbosityOverride LogCategory##CategoryName##Override(&CategoryName, ScopeVerbosity)
#endif