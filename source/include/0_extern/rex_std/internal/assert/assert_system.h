// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: assert_system.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/assert/assert_severity.h"
#include "rex_std/internal/assert/assert_id.h"

#include "rex_std/bonus/types.h"

#include "rex_std/internal/string/string_forward_declare.h"

namespace rsl
{
  namespace assert_system
  {
    string generate_assert_message(Severity severity, string_view expr, string_view file, string_view func_sig, card32 line_nr/*, string_view formattedMsg*/);
    bool rex_assert(Severity severity, bool expression, string&& log_message, AssertID assert_id, string_view file, string_view function_sig, card32 line_nr);
  }
}
