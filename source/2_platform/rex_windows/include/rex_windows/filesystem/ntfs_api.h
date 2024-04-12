#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace win32
  {
    struct WinUsnRecord;
    struct WinUsnJournalData;
    struct WinReadUsnJournalData;
    class UsnRecord;

    namespace ntfs
    {
      bool query_usn_journal(const rsl::win::handle& drive, rsl::Out<WinUsnJournalData> usnJournalData);
      WinUsnRecord* query_usn_record_array(const rsl::win::handle& drive, WinReadUsnJournalData& usnReadData, rsl::array<char8, 4096>& buf, rsl::Out<ulong> bytesRead);
      void parse_usn_record_array(WinUsnRecord* usnRecordArray, ulong bytesRead, rsl::Out<rsl::vector<UsnRecord>> outRecords);
      rsl::string parse_reason(ulong reason);
      rsl::string parse_file_attribs(ulong attribs);
      rsl::string get_filename(const rsl::win::handle& drive, uint64 fileReferenceNumber);

    } // namespace ntfs
  }   // namespace win32
} // namespace rex