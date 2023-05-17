#pragma once

#include "rex_engine/platform.h" // IWYU pragma: keep
#include "rex_std/array.h"       // for array
#include "rex_std/bonus/types.h" // for count_t, card32
#include "rex_std/stacktrace.h" // for stacktrace_entry

namespace rex
{
  namespace win
  {
    inline constexpr card32 g_max_stack_entries = 20;
    using CallStack                             = rsl::array<void*, g_max_stack_entries>;

    class ResolvedCallstack
    {
    public:
      explicit ResolvedCallstack(const CallStack& callstack);

      const rsl::array<rsl::stacktrace_entry, g_max_stack_entries>& pointers() const;
      count_t size() const;
      const rsl::stacktrace_entry& operator[](count_t idx) const;

    private:
      rsl::array<rsl::stacktrace_entry, g_max_stack_entries> m_resolved_stacktrace;
      count_t m_size;
    };

    CallStack current_callstack();
  } // namespace win
} // namespace rex
