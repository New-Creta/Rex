// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: assert_system.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/assert/assert_system.h"

#include "rex_std/internal/assert/assert_info.h"
#include "rex_std/internal/assert/tracked_assert_info.h"
#include "rex_std/internal/assert/assert_context.h"

#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/format.h"
#include "rex_std/vector.h"

namespace rsl
{
    namespace assert_system
    {
        rsl::vector<AssertContext> g_contexts;

        rsl::string generate_assert_message(Severity /*severity*/, rsl::string_view /*expr*/, rsl::string_view /*file*/, rsl::string_view /*func_sig*/, card32 /*line_nr*//*, rsl::string_view*/ /*formattedMsg*/)
        {
            //static rsl::string msg;
            //msg.clear();

            //msg = fmt::format("{} message\n{}({}) {}\n", rsl::enum_name(severity), file, line_nr, func_sig);

            //for (const rsl::string_view context : g_contexts)
            //{
            //  msg += fmt::format("[context] {}\n");
            //}

            //msg += fmt::format("Expression that failed: {}\n");
            //msg += formattedMsg;

            //return msg;
            return string("");
        }

        bool rex_assert(Severity /*severity*/, bool /*expression*/, rsl::string&& /*log_message*/, AssertID /*assert_id*/, rsl::string_view /*file*/, rsl::string_view /*function_sig*/, card32 /*line_nr*/)
        {
            return true;
            //  static int initialized_check = []()
            //  {
            //    if (!is_initialized())
            //    {
            //      REX_LOG("Assert system not initialized yet");
            //      REX_DEBUG_BREAK();
            //    }
            //    return 0;
            //  }();

            //  if (!expression)
            //  {
            //    // Callstack callstack = should_report_callstack(severity)
            //    //   ? stack_trace(1);
            //    //   : Callstack::empty();
            //    AssertInfo assert_info(severity, assert_id, rsl::move(log_message), file, function_sig, line_nr /*, callstack*/);

            //    log_assert(assert_info.msg());
            //    popup_assert(severity, popup_message);

            //    add_to_assert_db(rsl::move(assert_info));

            //    if (should_break(severity)) REX_UNLIKELY
            //    {
            //      REX_DEBUG_BREAK();
            //    }
            //  }
            //  return !expression;
            //}
          //}
        }
    }
}