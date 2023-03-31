#include "rex_engine/diagnostics/logging/log_category.h"

namespace rex
{
  //-------------------------------------------------------------------------
  LogCategoryBase::LogCategoryBase(const LogCategoryName& inCategoryName, LogVerbosity inDefaultVerbosity)
      : default_verbosity_(inDefaultVerbosity)
      , category_name_(inCategoryName)
  {
    // LOG CATEGORY? (CategoryName, InDefaultVerbosity);
    resetFromDefault();
  }

  //-------------------------------------------------------------------------
  LogCategoryBase::~LogCategoryBase() = default;

  //-------------------------------------------------------------------------
  bool LogCategoryBase::isSuppressed(LogVerbosity level) const
  {
    if((level & LogVerbosity::VerbosityMask) <= verbosity_)
    {
      return false;
    }

    return true;
  }

  //-------------------------------------------------------------------------
  const LogCategoryName& LogCategoryBase::getCategoryName() const
  {
    return category_name_;
  }

  //-------------------------------------------------------------------------
  const LogVerbosity& LogCategoryBase::getVerbosity() const
  {
    return verbosity_;
  }

  //-------------------------------------------------------------------------
  void LogCategoryBase::setVerbosity(LogVerbosity newVerbosity)
  {
    const LogVerbosity old_verbosity = verbosity_;

    verbosity_ = newVerbosity & LogVerbosity::VerbosityMask;
  }

  //-------------------------------------------------------------------------
  void LogCategoryBase::resetFromDefault()
  {
    setVerbosity(default_verbosity_);
  }
} // namespace rex