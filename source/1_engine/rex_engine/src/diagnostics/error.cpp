#include "rex_engine/diagnostics/error.h"

namespace rex
{
  Error::Error(rsl::string_view errorMsg, s32 errorCode)
      : m_error_msg(errorMsg)
      , m_error_code(errorCode)
  {
  }

  Error::operator bool() const
  {
    return has_error();
  }
  bool Error::has_error() const
  {
    return m_error_code != 0;
  }
  rsl::string_view Error::error_msg() const
  {
    return m_error_msg;
  }
  s32 Error::error_code() const
  {
    return m_error_code;
  }

  Error Error::no_error()
  {
    return Error("", 0);
  }
} // namespace rex