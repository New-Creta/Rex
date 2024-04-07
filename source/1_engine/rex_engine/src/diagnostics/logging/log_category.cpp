#include "rex_engine/diagnostics/logging/log_category.h"

#include "rex_engine/diagnostics/logging/log_functions.h"

namespace rex
{
  //-------------------------------------------------------------------------
  LogCategory::LogCategory(const LogCategoryName& inCategoryName, IsAsync inAsync)
      : m_category_name(inCategoryName)
      , m_is_async(inAsync)
  {
  }

  //-------------------------------------------------------------------------
  bool LogCategory::is_suppressed(log::LogVerbosity level) const // NOLINT(readability-convert-member-functions-to-static)
  {
    return log::is_supressed(level);
  }

  //-------------------------------------------------------------------------
  bool LogCategory::is_async() const
  {
    return m_is_async;
  }

  //-------------------------------------------------------------------------
  rsl::string_view LogCategory::get_category_name() const
  {
    return m_category_name;
  }
} // namespace rex