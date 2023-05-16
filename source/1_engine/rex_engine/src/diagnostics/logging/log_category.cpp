#include "rex_engine/diagnostics/logging/log_category.h"

#include "rex_engine/diagnostics/logging/log_verbosity.h" // for operator&

namespace rex
{
  //-------------------------------------------------------------------------
  LogCategory::LogCategory(const LogCategoryName& inCategoryName, LogVerbosity inDefaultVerbosity)
      : m_verbosity(inDefaultVerbosity & LogVerbosity::VerbosityMask)
      , m_category_name(inCategoryName)
  {
    // LOG CATEGORY? (CategoryName, InDefaultVerbosity);
  }

  //-------------------------------------------------------------------------
  bool LogCategory::is_suppressed(LogVerbosity level) const
  {
    return (level & LogVerbosity::VerbosityMask) > m_verbosity;
  }

  //-------------------------------------------------------------------------
  const LogCategoryName& LogCategory::get_category_name() const
  {
    return m_category_name;
  }

  //-------------------------------------------------------------------------
  const LogVerbosity& LogCategory::get_verbosity() const
  {
    return m_verbosity;
  }

  //-------------------------------------------------------------------------
  void LogCategory::set_verbosity(LogVerbosity newVerbosity)
  {
    m_verbosity = newVerbosity & LogVerbosity::VerbosityMask;
  }
} // namespace rex