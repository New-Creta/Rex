#include "rex_engine/diagnostics/logging/log_category.h"

namespace rex
{
  //-------------------------------------------------------------------------
  LogCategory::LogCategory(const LogCategoryName& inCategoryName, LogVerbosity inDefaultVerbosity, IsAsync async)
      : m_verbosity(inDefaultVerbosity & LogVerbosity::VerbosityMask)
      , m_category_name(inCategoryName)
      , m_is_async(async)
  {
  }

  //-------------------------------------------------------------------------
  bool LogCategory::is_suppressed(LogVerbosity level) const
  {
    return (level & LogVerbosity::VerbosityMask) > m_verbosity;
  }

  //-------------------------------------------------------------------------
  bool LogCategory::is_async() const
  {
      return m_is_async == IsAsync::yes;
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