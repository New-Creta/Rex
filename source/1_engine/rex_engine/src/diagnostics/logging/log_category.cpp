#include "rex_engine/diagnostics/logging/log_category.h"

namespace rex
{
  //-------------------------------------------------------------------------
  LogCategoryBase::LogCategoryBase(const LogCategoryName& inCategoryName, LogVerbosity inDefaultVerbosity)
      : m_default_verbosity(inDefaultVerbosity)
      , m_category_name(inCategoryName)
  {
    // LOG CATEGORY? (CategoryName, InDefaultVerbosity);
    reset_from_default();
  }

  //-------------------------------------------------------------------------
  LogCategoryBase::~LogCategoryBase() = default;

  //-------------------------------------------------------------------------
  bool LogCategoryBase::is_suppressed(LogVerbosity level) const
  {
    if((level & LogVerbosity::VerbosityMask) <= m_verbosity)
    {
      return false;
    }

    return true;
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
    const LogVerbosity old_verbosity = m_verbosity;

    m_verbosity = newVerbosity & LogVerbosity::VerbosityMask;
  }

  //-------------------------------------------------------------------------
  void LogCategoryBase::reset_from_default()
  {
    set_verbosity(m_default_verbosity);
  }
} // namespace rex