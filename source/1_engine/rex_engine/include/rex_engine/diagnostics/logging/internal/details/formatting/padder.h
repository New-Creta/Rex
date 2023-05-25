#pragma once

#include "rex_engine/diagnostics/logging/internal/details/formatting/padding_info.h"
#include "rex_engine/diagnostics/logging/internal/details/fmt_helper.h"

namespace rexlog
{
    namespace details
    {
        class ScopedPadder
        {
        public:
            ScopedPadder(size_t wrapped_size, const PaddingInfo& padinfo, memory_buf_t& dest);

            ScopedPadder(const ScopedPadder&) = delete;
            ScopedPadder(ScopedPadder&&) = delete;

            ~ScopedPadder();

            ScopedPadder& operator=(const ScopedPadder&) = delete;
            ScopedPadder& operator=(ScopedPadder&&) = delete;

            template <typename T>
            static unsigned int count_digits(T d)
            {
                return fmt_helper::count_digits(d);
            }

        private:
            void pad_it(ulong count);

            const PaddingInfo*  m_padinfo;
            memory_buf_t*       m_dest;
            ulong               m_remaining_pag;
            string_view_t       m_spaces;
        };

        class NullScopedPadder
        {
        public:
            NullScopedPadder(size_t /*wrapped_size*/, const PaddingInfo& /*padinfo*/, memory_buf_t& /*dest*/);

            template <typename T>
            static unsigned int count_digits(T /* number */)
            {
                return 0;
            }
        };
    }
}