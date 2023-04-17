#pragma once

#include "rex_std/assert.h"
#include "rex_std/format.h"
#include "rex_std/source_location.h"

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
    AssertContextScope(const rsl::fmt_stack_string& msg, rsl::source_location sourceLoc = rsl::source_location::current());
    ~AssertContextScope();
  };
}

#define REX_ASSERT(...)         rex::rex_assert(rsl::format(__VA_ARGS__)); while (true) { DEBUG_BREAK(); }
#define REX_ASSERT_X(cond, ...) if (!(cond)) { REX_ASSERT(__VA_ARGS__); }

#define REX_ASSERT_CONTEXT_SCOPE(...) rex::AssertContextScope ANONYMOUS_VARIABLE(assert_scope)(rsl::format(__VA_ARGS__));