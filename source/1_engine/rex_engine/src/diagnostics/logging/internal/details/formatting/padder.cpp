#include "rex_engine/diagnostics/logging/internal/details/formatting/padder.h"

namespace rexlog
{
    namespace details
    {
        ///////////////////////////////////////////////////////////////////////
        // Scoped Padder
        ///////////////////////////////////////////////////////////////////////
        //-------------------------------------------------------------------------
        ScopedPadder::ScopedPadder(size_t wrapped_size, const PaddingInfo& padinfo, memory_buf_t& dest)
            : m_padinfo(&padinfo)
            , m_dest(&dest)
            , m_remaining_pag(static_cast<long>(padinfo.width) - static_cast<long>(wrapped_size))
            , m_spaces({ "                                                                ", 64 })
        {
            if (m_remaining_pag <= 0)
            {
                return;
            }

            if (m_padinfo->side == PaddingInfo::PadSide::Left)
            {
                pad_it(m_remaining_pag);
                m_remaining_pag = 0;
            }
            else if (m_padinfo->side == PaddingInfo::PadSide::Center)
            {
                auto half_pad = m_remaining_pag / 2;
                auto reminder = m_remaining_pag & 1u;
                pad_it(static_cast<long>(half_pad));
                m_remaining_pag = static_cast<ulong>(half_pad + reminder); // for the Right side
            }
        }

        //-------------------------------------------------------------------------
        ScopedPadder::~ScopedPadder()
        {
            if (m_remaining_pag >= 0)
            {
                pad_it(m_remaining_pag);
            }
            else if (m_padinfo->truncate)
            {
                const ulong new_size = m_dest->size() + m_remaining_pag;
                m_dest->resize(static_cast<count_t>(new_size));
            }
        }

        //-------------------------------------------------------------------------
        void ScopedPadder::pad_it(ulong count)
        {
            fmt_helper::append_string_view(string_view_t(m_spaces.data(), static_cast<count_t>(count)), *m_dest);
        }

        ///////////////////////////////////////////////////////////////////////
        // Null Scoped Padder
        ///////////////////////////////////////////////////////////////////////
        //-------------------------------------------------------------------------
        NullScopedPadder::NullScopedPadder(size_t /*wrapped_size*/, const PaddingInfo& /*padinfo*/, memory_buf_t& /*dest*/)
        {

        }
    }
}