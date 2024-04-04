#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/diagnostics/stacktrace.h"
#include "rex_engine/engine/debug_types.h"
#include "rex_engine/engine/defines.h"
#include "rex_std/bonus/types.h"

namespace rex
{
  DEFINE_LOG_CATEGORY(LogAssert);

  DebugVector<AssertContext>& contexts()
  {
    thread_local DebugVector<AssertContext> contexts;
    return contexts;
  }

  void rex_assert(const rsl::fmt_stack_string& msg)
  {
    thread_local static bool is_processing_assert = false;
    if(!is_processing_assert)
    {
      is_processing_assert = true;
      REX_ERROR(LogAssert, "Assert Raised: {}", msg);

      REX_ERROR(LogAssert, "Assert contexts:");
      REX_ERROR(LogAssert, "----------------");

      for(const AssertContext& context: contexts())
      {
        REX_ERROR(LogAssert, "{}", context.msg());
        REX_ERROR(LogAssert, "[traceback] {}", rsl::to_string(context.source_location()));
      }

      REX_ERROR(LogAssert, "----------------");

      const ResolvedCallstack callstack(current_callstack());

      for(count_t i = 0; i < callstack.size(); ++i)
      {
        REX_ERROR(LogAssert, "{}", callstack[i]);
      }
    }
    else
    {
      // if this is hit, an assert occurred while processing another one.
      // to avoid circular dependency, we break here if there's a debugger attached
      DEBUG_BREAK();
    }
  }

  void push_assert_context(const rsl::fmt_stack_string& msg, rsl::source_location sourceLoc)
  {
    contexts().emplace_back(msg, sourceLoc);
  }
  void pop_assert_context()
  {
    contexts().pop_back();
  }

  AssertContext::AssertContext(const rsl::fmt_stack_string& msg, rsl::source_location sourceLoc)
      : m_msg(msg)
      , m_source_location(sourceLoc)
  {
  }

  const rsl::fmt_stack_string& AssertContext::msg() const
  {
    return m_msg;
  }
  const rsl::source_location& AssertContext::source_location() const
  {
    return m_source_location;
  }

  AssertContextScope::AssertContextScope(const rsl::fmt_stack_string& msg, rsl::source_location sourceLoc)
  {
    push_assert_context(msg, sourceLoc);
  }

  AssertContextScope::~AssertContextScope()
  {
    pop_assert_context();
  }
} // namespace rex