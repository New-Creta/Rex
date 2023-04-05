#include "rex_engine/diagnostics/logging/log_scoped_verbosity_override.h"

#include "rex_engine/diagnostics/logging/log_category.h"

namespace rex
{
  //-------------------------------------------------------------------------
  /** Back up the existing verbosity for the category then sets new verbosity.*/
  LogScopedVerbosityOverride::LogScopedVerbosityOverride(LogCategoryBase* category, LogVerbosity verbosity)
      : saved_category_(category)
  {
    // assert(saved_category_ != nullptr);

    saved_verbosity_ = saved_category_->get_verbosity();
    saved_category_->set_verbosity(verbosity);
  }

  //-------------------------------------------------------------------------
  /** Restore the verbosity overrides for the category to the previous value.*/
  LogScopedVerbosityOverride::~LogScopedVerbosityOverride()
  {
    saved_category_->set_verbosity(saved_verbosity_);
  }
} // namespace rex