#pragma once

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

namespace rex
{
  class Error
  {
  public:
    Error(rsl::string_view errorMsg, s32 errorCode = 1);

    explicit operator bool() const;
    bool has_error() const;

    rsl::string_view error_msg() const;
    s32 error_code() const;

    static Error no_error();

  private:
    rsl::medium_stack_string m_error_msg;
    s32 m_error_code;
  };
}