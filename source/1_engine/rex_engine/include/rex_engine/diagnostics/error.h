#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/string.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_std/format.h"

namespace rex
{
  // An error class can be used as an return value from a function to indicate an error
  // Use Error::no_error to return an object that indicates no error has ocurred
  // Otherwise invoke the constructor to set an error message and an error code
  // Anything code other than 0 is treated as an error
  class Error
  {
  public:
    explicit Error(rsl::string_view errorMsg, s32 errorCode = 1);

    // Returns true on error
    explicit operator bool() const;
    bool has_error() const;

    rsl::string_view error_msg() const;
    s32 error_code() const;

    // Returns an Error object that indicates no error
    static Error no_error();

    // Creates an error object and logs its error msg
    template <typename ... Args>
    static Error create_with_log(LogCategory category, rsl::string_view msg, Args&& ... args)
    {
      REX_UNUSED_PARAM(category);

      auto fmt_string = rsl::format(msg, rsl::forward<Args>(args)...);
      REX_ERROR(category, fmt_string);
      return Error(fmt_string);
    }

  private:
    rsl::medium_stack_string m_error_msg;
    s32 m_error_code;
  };
} // namespace rex