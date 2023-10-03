#include "rex_windows/filesystem/usn_journal.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_std/iostream.h"
#include "rex_std/set.h"
#include "rex_std/utility.h"
#include "rex_windows/filesystem/internal/ntfs_types.h"
#include "rex_windows/filesystem/ntfs_api.h"
#include "rex_windows/filesystem/usn_record.h"

DEFINE_LOG_CATEGORY(LogNtfsJournal, rex::LogVerbosity::Log);

namespace rex
{
  namespace win32
  {
    UsnJournal::UsnJournal(DriveHandle&& drive, rsl::string_view lastUsnFilePath)
        : m_last_usn_filepath(lastUsnFilePath)
        , m_drive_handle(rsl::move(drive))
        , m_journal_data(rsl::make_unique<WinUsnJournalData>())
        , m_last_usn(load_last_usn())
    {
      init_journal_data(m_drive_handle);
    }

    UsnJournal::~UsnJournal()
    {
      save_last_usn();
    }

    rsl::vector<rsl::string> UsnJournal::read()
    {
      // Prepare ReadUsnJournalData
      WinReadUsnJournalData usn_read_data = {};
      usn_read_data.StartUsn              = m_last_usn;
      usn_read_data.ReasonMask            = 0xFFFFFFFF;
      usn_read_data.UsnJournalID          = m_journal_data->UsnJournalID;
      usn_read_data.BytesToWaitFor        = 200;
      usn_read_data.Timeout               = 5;

      rsl::array<char8, 4096> buf;
      ulong bytes_read = 0;
      uint64 last_usn  = 0;

      rsl::vector<UsnRecord> records;

      // Iterate over journal
      while(true)
      {
        buf.fill(0);

        WinUsnRecord* record_array = ntfs::query_usn_record_array(m_drive_handle.handle(), usn_read_data, buf, rsl::Out(bytes_read));
        if(!record_array)
        {
          if(bytes_read == 0)
          {
            REX_LOG(LogNtfsJournal, "Failed to read USN Journal for {} : error({})", m_journal_data->UsnJournalID, GetLastError());
          }
          break;
        }

        ntfs::parse_usn_record_array(record_array, bytes_read, rsl::Out(records));
        last_usn = records.back().usn();

        usn_read_data.StartUsn = *(USN*)buf.data();
      }

      m_last_usn = last_usn;

      // Create the result to be returned
      rsl::set<rsl::string> files;

      rsl::string filename;
      for(const UsnRecord& record: records)
      {
        filename = ntfs::get_filename(m_drive_handle.handle(), record.file_ref_nr());
        if(files.count(filename) == 0 && filename.length() > 0)
        {
          files.emplace(filename);
        }
      }

      rsl::vector<rsl::string> result;
      result.reserve(files.size());

      for(rsl::string_view file: files)
      {
        result.emplace_back(file);
      }

      return result;
    }

    bool UsnJournal::init_journal_data(DriveHandle& drive)
    {
      bool res = ntfs::query_usn_journal(drive.handle(), rsl::Out(*m_journal_data));
      if(!res)
      {
        DWORD err = GetLastError();

        switch(err)
        {
          case ERROR_JOURNAL_NOT_ACTIVE: REX_LOG(LogNtfsJournal, "Drive's journal hasn't been activated. Drive: {}", m_drive_handle.volume_path()); break;

          default: REX_LOG(LogNtfsJournal, "Couldn't query USN journal with error: {}", err); break;
        }
      }

      return res;
    }

    uint64 UsnJournal::load_last_usn()
    {
      if(!rex::vfs::exists(m_last_usn_filepath))
      {
        return 0;
      }

      rex::memory::Blob blob = rex::vfs::open_read(m_last_usn_filepath);
      return blob.read<uint64>();
    }

    void UsnJournal::save_last_usn()
    {
      rex::vfs::save_to_file(m_last_usn_filepath, &m_last_usn, sizeof(m_last_usn), rex::vfs::AppendToFile::no);
    }
  } // namespace win32
} // namespace rex