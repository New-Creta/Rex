#include "rex_engine/diagnostics/logging/log_scoped_verbosity_override.h"

#include "rex_engine/diagnostics/logging/log_category.h"
#include "rex_engine/diagnostics/logging/log_functions.h"

#include "rex_engine/diagnostics/logging/internal/details/registry.h"

namespace rex
{
  //-------------------------------------------------------------------------
  /** Back up the existing verbosity for the category then sets new verbosity.*/
  LogScopedVerbosityOverride::LogScopedVerbosityOverride(LogCategory* category, log::LogVerbosity verbosity)
      : m_saved_category(category)
      , m_saved_log_level(log::details::Registry::instance().get_global_level())
  {
    log::details::Registry::instance().set_level(verbosity);
  }

  //-------------------------------------------------------------------------
  /** Restore the verbosity overrides for the category to the previous value.*/
  LogScopedVerbosityOverride::~LogScopedVerbosityOverride()
  {
    log::details::Registry::instance().set_level(m_saved_log_level);
  }
} // namespace rex