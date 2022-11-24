// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: assert_info.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/assert/assert_severity.h"
#include "rex_std/internal/assert/assert_id.h"

#include "rex_std/string.h"
#include "rex_std/string_view.h"

namespace rsl
{
  class AssertInfo
  {
  public:
    AssertInfo(Severity severity, AssertID assert_id, rsl::string&& msg, rsl::string_view file, rsl::string_view func_sig, card32 line_nr);
    
    Severity severity() const;
    AssertID assert_id() const;
    rsl::string_view msg() const;
    rsl::string_view file() const;
    rsl::string_view function() const;

  private:
    Severity m_severity;
    AssertID m_assert_id;
    rsl::string m_message;
    rsl::string_view m_file;
    rsl::string_view m_func_sig;
    card32 m_line_nr;
  };
}