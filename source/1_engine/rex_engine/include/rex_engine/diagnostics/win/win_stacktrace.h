#pragma once

#include "rex_engine/platform.h" // IWYU pragma: keep
#include "rex_std/array.h"
#include "rex_std/bonus/types.h"
#include "rex_std/iostream.h"
#include "rex_std/stacktrace.h"

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

    rsl::ostream& operator<<(rsl::ostream& os, const ResolvedCallstack& callstack);

    CallStack current_callstack();
  } // namespace win
} // namespace rex

namespace rsl
{
  inline namespace v1
  {
    template <>
    struct hash<rex::win::CallStack>
    {
      rsl::hash_result operator()(const rex::win::CallStack& callstack) const
      {
        const count_t size = callstack.size();
        card64 seed        = 0;
        for(count_t i = 0; i < size; ++i)
        {
          void* stack_pointer  = callstack[i];
          const intptr address = *reinterpret_cast<intptr*>(&stack_pointer); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
          seed                 = internal::hash_combine(seed, address);
        }

        return static_cast<rsl::hash_result>(seed);
      }
    };
  } // namespace v1
} // namespace rsl
