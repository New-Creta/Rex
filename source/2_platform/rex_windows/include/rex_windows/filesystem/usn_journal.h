#pragma once

#include "rex_std/bonus/platform.h"
#include "rex_std/string.h"
#include "rex_std/vector.h"
#include "rex_windows/filesystem/drive_handle.h"

namespace rex
{
  namespace win32
  {
    struct WinUsnJournalData;

    class UsnJournal
    {
    public:
      UsnJournal(DriveHandle&& drive, rsl::string_view lastUsnFilePath);
      ~UsnJournal();

      rsl::vector<rsl::string> read();

    private:
      bool init_journal_data(DriveHandle& drive);
      uint64 load_last_usn();
      void save_last_usn();

    private:
      rsl::medium_stack_string m_last_usn_filepath;
      DriveHandle m_drive_handle;
      rsl::unique_ptr<WinUsnJournalData> m_journal_data;
      uint64 m_last_usn;
    };
  } // namespace win32
} // namespace rex