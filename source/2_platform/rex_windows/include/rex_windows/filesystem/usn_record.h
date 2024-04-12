#pragma once

#include "rex_engine/types.h"
#include "rex_std/string.h"
#include "rex_std_extra/time/timepoint.h"

namespace rex
{
  namespace win32
  {
    struct WinUsnRecord;

    class UsnRecord
    {
    public:
      UsnRecord(WinUsnRecord* usnRecord);

      uint64 usn() const;
      uint64 file_ref_nr() const;

    private:
      rsl::time_point m_timepoint;
      WinUsnRecord* m_usn_record;
      rsl::string m_reason;
      rsl::string m_file_attribs;
      uint64 m_mft_idx;
    };
  } // namespace win32
} // namespace rex