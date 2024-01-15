#pragma once

#include "rex_engine/diagnostics/logging/internal/details/fmt_helper.h"
#include "rex_engine/diagnostics/logging/internal/details/formatting/padding_info.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/string.h"

namespace rexlog
{
  namespace details
  {
    class ScopedPadder
    {
    public:
      ScopedPadder(s32 wrappedSize, const PaddingInfo& padinfo, rsl::big_stack_string& dest);

      ScopedPadder(const ScopedPadder&) = delete;
      ScopedPadder(ScopedPadder&&)      = delete;

      ~ScopedPadder();

      ScopedPadder& operator=(const ScopedPadder&) = delete;
      ScopedPadder& operator=(ScopedPadder&&)      = delete;

      template <typename T>
      static unsigned int count_digits(T d)
      {
        return fmt_helper::count_digits(d);
      }

    private:
      void pad_it(ulong count);

      const PaddingInfo* m_padinfo;
      rsl::big_stack_string* m_dest;
      ulong m_remaining_pag;
      rsl::string_view m_spaces;
    };

    class NullScopedPadder
    {
    public:
      NullScopedPadder(s32 /*wrappedSize*/, const PaddingInfo& /*padinfo*/, rsl::big_stack_string& /*dest*/);

      template <typename T>
      static unsigned int count_digits(T /* number */)
      {
        return 0;
      }
    };
  } // namespace details
} // namespace rexlog