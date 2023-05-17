#pragma once

#include "rex_std/bonus/defines.h"   // for ANONYMOUS_VARIABLE, DEBUG_BREAK
#include "rex_std/format.h"          // for fmt_stack_string
#include "rex_std/source_location.h" // for source_location

namespace rex
{
  void rex_assert(const rsl::fmt_stack_string& msg);
  void push_assert_context(const rsl::fmt_stack_string& msg, rsl::source_location sourceLoc);
  void pop_assert_context();

  class AssertContext
  {
  public:
    AssertContext(const rsl::fmt_stack_string& msg, rsl::source_location sourceLoc);

    const rsl::fmt_stack_string& msg() const;
    const rsl::source_location& source_location() const;

  private:
    rsl::fmt_stack_string m_msg;
    rsl::source_location m_source_location;
  };

  class AssertContextScope
  {
  public:
    explicit AssertContextScope(const rsl::fmt_stack_string& msg, rsl::source_location sourceLoc = rsl::source_location::current());
    ~AssertContextScope();

    AssertContextScope(const AssertContextScope&) = delete;
    AssertContextScope(AssertContextScope&&)      = delete;

    AssertContextScope& operator=(const AssertContextScope&) = delete;
    AssertContextScope& operator=(AssertContextScope&&)      = delete;
  };
} // namespace rex

#ifdef REX_ENABLE_ASSERTS
  #define REX_ASSERT(...)                                                                                                                                                                                                                                \
    rex::rex_assert(rsl::format(__VA_ARGS__));                                                                                                                                                                                                           \
    while(true)                                                                                                                                                                                                                                          \
    {                                                                                                                                                                                                                                                    \
      DEBUG_BREAK();                                                                                                                                                                                                                                     \
    }
  #define REX_ASSERT_X(cond, ...)                                                                                                                                                                                                                        \
    if(!(cond))                                                                                                                                                                                                                                          \
    {                                                                                                                                                                                                                                                    \
      REX_ASSERT(__VA_ARGS__);                                                                                                                                                                                                                           \
    }

  #define REX_ASSERT_CONTEXT_SCOPE(...) const rex::AssertContextScope ANONYMOUS_VARIABLE(assert_scope)(rsl::format(__VA_ARGS__));

#else
  #define REX_ASSERT(...)
  #define REX_ASSERT_X(cond, ...)
  #define REX_ASSERT_CONTEXT_SCOPE(...)
#endif