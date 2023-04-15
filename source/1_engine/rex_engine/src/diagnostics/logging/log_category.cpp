#include "rex_engine/diagnostics/logging/log_category.h"

namespace rex
{
  //-------------------------------------------------------------------------
  LogCategoryBase::LogCategoryBase(const LogCategoryName& inCategoryName, LogVerbosity inDefaultVerbosity)
      : m_verbosity(inDefaultVerbosity & LogVerbosity::VerbosityMask)
      , m_category_name(inCategoryName)
  {
    // LOG CATEGORY? (CategoryName, InDefaultVerbosity);
  }

  //-------------------------------------------------------------------------
  bool LogCategoryBase::is_suppressed(LogVerbosity level) const
  {
    return (level & LogVerbosity::VerbosityMask) > m_verbosity;
  }

  //-------------------------------------------------------------------------
  const LogCategoryName& LogCategoryBase::get_category_name() const
  {
    return m_category_name;
  }

  //-------------------------------------------------------------------------
  const LogVerbosity& LogCategoryBase::get_verbosity() const
  {
    return m_verbosity;
  }

  //-------------------------------------------------------------------------
  void LogCategoryBase::set_verbosity(LogVerbosity newVerbosity)
  {
    m_verbosity = newVerbosity & LogVerbosity::VerbosityMask;
  }
} // namespace rex