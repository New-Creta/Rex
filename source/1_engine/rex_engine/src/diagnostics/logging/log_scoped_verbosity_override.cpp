#include "rex_engine/diagnostics/logging/log_scoped_verbosity_override.h"

#include "rex_engine/diagnostics/logging/log_category.h"  // for LogCategory

namespace rex
{
enum class LogVerbosity;

  //-------------------------------------------------------------------------
  /** Back up the existing verbosity for the category then sets new verbosity.*/
  LogScopedVerbosityOverride::LogScopedVerbosityOverride(LogCategory* category, LogVerbosity verbosity)
      : m_saved_category(category)
      , m_saved_verbosity(m_saved_category->get_verbosity())
  {
    // assert(saved_category_ != nullptr);

    m_saved_category->set_verbosity(verbosity);
  }

  //-------------------------------------------------------------------------
  /** Restore the verbosity overrides for the category to the previous value.*/
  LogScopedVerbosityOverride::~LogScopedVerbosityOverride()
  {
    m_saved_category->set_verbosity(m_saved_verbosity);
  }
} // namespace rex