#include "rex_windows/filesystem/usn_record.h"

#include "rex_windows/filesystem/internal/ntfs_types.h"
#include "rex_windows/filesystem/ntfs_api.h"

namespace rex
{
  namespace win32
  {
    UsnRecord::UsnRecord(WinUsnRecord* usnRecord)
        : m_timepoint()
        , m_usn_record(usnRecord)
        , m_reason()
        , m_file_attribs()
        , m_mft_idx()
    {
      FILETIME ft = {};

      ft.dwLowDateTime  = m_usn_record->TimeStamp.LowPart;
      ft.dwHighDateTime = m_usn_record->TimeStamp.HighPart;

      SYSTEMTIME st;
      FileTimeToSystemTime(&ft, &st);
      m_timepoint = rsl::timepoint_from_systime(st);

      m_mft_idx = m_usn_record->FileReferenceNumber;
      m_mft_idx &= 0x0000ffffffffffff;

      m_reason       = ntfs::parse_reason(m_usn_record->Reason);
      m_file_attribs = ntfs::parse_file_attribs(m_usn_record->FileAttributes);
    }

    uint64 UsnRecord::usn() const
    {
      return m_usn_record->Usn;
    }

    uint64 UsnRecord::file_ref_nr() const
    {
      return m_usn_record->FileReferenceNumber;
    }
  } // namespace win32
} // namespace rex