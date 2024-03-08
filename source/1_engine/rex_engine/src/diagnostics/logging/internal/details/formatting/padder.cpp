#include "rex_engine/diagnostics/logging/internal/details/formatting/padder.h"

#include "rex_std/bonus/types.h"

// IWYU pragma: no_include <built-in>

namespace rex
{
  namespace log
  {
    namespace details
    {
      ///////////////////////////////////////////////////////////////////////
      // Scoped Padder
      ///////////////////////////////////////////////////////////////////////
      //-------------------------------------------------------------------------
      ScopedPadder::ScopedPadder(s32 wrappedSize, const PaddingInfo& padinfo, rsl::string& dest)
          : m_padinfo(&padinfo)
          , m_dest(&dest)
          , m_remaining_pag(static_cast<long>(padinfo.width) - static_cast<long>(wrappedSize))
          , m_spaces({"                                                                ", 64})
      {
        if(m_remaining_pag <= 0)
        {
          return;
        }

        if(m_padinfo->side == PaddingInfo::PadSide::Left)
        {
          pad_it(m_remaining_pag);
          m_remaining_pag = 0;
        }
        else if(m_padinfo->side == PaddingInfo::PadSide::Center)
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
        if(m_remaining_pag >= 0)
        {
          pad_it(m_remaining_pag);
        }
        else if(m_padinfo->truncate)
        {
          const ulong new_size = m_dest->size() + m_remaining_pag;
          m_dest->resize(static_cast<count_t>(new_size));
        }
      }

      //-------------------------------------------------------------------------
      void ScopedPadder::pad_it(ulong count)
      {
        fmt_helper::append_string_view(rsl::string_view(m_spaces.data(), static_cast<count_t>(count)), *m_dest);
      }

      ///////////////////////////////////////////////////////////////////////
      // Null Scoped Padder
      ///////////////////////////////////////////////////////////////////////
      //-------------------------------------------------------------------------
      NullScopedPadder::NullScopedPadder(s32 /*wrappedSize*/, const PaddingInfo& /*padinfo*/, rsl::string& /*dest*/) {}
    } // namespace details
  }   // namespace log
} // namespace rex