#include "rex_windows/filesystem/ntfs_api.h"

#include "rex_windows/filesystem/internal/ntfs_types.h"
#include "rex_windows/filesystem/usn_record.h"

namespace rex
{
  namespace win32
  {
    namespace ntfs
    {
      bool query_usn_journal(const rsl::win::handle& drive, rsl::Out<WinUsnJournalData> usnJournalData)
      {
        return DeviceIoControl(drive.get(), FSCTL_QUERY_USN_JOURNAL, NULL, 0, &usnJournalData.get(), sizeof(usnJournalData.get()), nullptr, NULL);
      }

      WinUsnRecord* query_usn_record_array(const rsl::win::handle& drive, WinReadUsnJournalData& usnReadData, rsl::array<char8, 4096>& buf, rsl::Out<ulong> bytesRead)
      {
        if(!DeviceIoControl(drive.get(), FSCTL_READ_UNPRIVILEGED_USN_JOURNAL, &usnReadData, sizeof(usnReadData), buf.data(), buf.size(), &bytesRead.get(), NULL))
        {
          return nullptr;
        }

        if(bytesRead.get() < 256)
        {
          return nullptr;
        }

        bytesRead.get() -= sizeof(USN);

        // Find the first record in the journal
        return (WinUsnRecord*)(((PUCHAR)buf.data()) + sizeof(USN));
      }

      void parse_usn_record_array(WinUsnRecord* usnRecordArray, ulong bytesRead, rsl::Out<rsl::vector<UsnRecord>> outRecords)
      {
        WinUsnRecord* usn_record = usnRecordArray;

        outRecords.get().clear();
        outRecords.get().reserve(bytesRead / usn_record->RecordLength);

        while(bytesRead > 0)
        {
          outRecords.get().emplace_back(usn_record);

          bytesRead -= usn_record->RecordLength;

          // Find the next record
          usn_record = (WinUsnRecord*)(((PCHAR)usn_record) + usn_record->RecordLength);
        }
      }

      rsl::string parse_reason(ulong reason)
      {
        char8 sz_reason[2048] = "";

        if(reason & USN_REASON_BASIC_INFO_CHANGE)
          strcat_s(sz_reason, "\t\tUSN_REASON_BASIC_INFO_CHANGE |\n");
        if(reason & USN_REASON_CLOSE)
          strcat_s(sz_reason, "\t\tUSN_REASON_CLOSE |\n");
        if(reason & USN_REASON_COMPRESSION_CHANGE)
          strcat_s(sz_reason, "\t\tUSN_REASON_COMPRESSION_CHANGE |\n");
        if(reason & USN_REASON_DATA_EXTEND)
          strcat_s(sz_reason, "\t\tUSN_REASON_DATA_EXTEND |\n");
        if(reason & USN_REASON_DATA_OVERWRITE)
          strcat_s(sz_reason, "\t\tUSN_REASON_DATA_OVERWRITE |\n");
        if(reason & USN_REASON_DATA_TRUNCATION)
          strcat_s(sz_reason, "\t\tUSN_REASON_DATA_TRUNCATION |\n");
        if(reason & USN_REASON_EA_CHANGE)
          strcat_s(sz_reason, "\t\tUSN_REASON_EA_CHANGE |\n");
        if(reason & USN_REASON_ENCRYPTION_CHANGE)
          strcat_s(sz_reason, "\t\tUSN_REASON_ENCRYPTION_CHANGE |\n");
        if(reason & USN_REASON_FILE_CREATE)
          strcat_s(sz_reason, "\t\tUSN_REASON_FILE_CREATE |\n");
        if(reason & USN_REASON_FILE_DELETE)
          strcat_s(sz_reason, "\t\tUSN_REASON_FILE_DELETE |\n");
        if(reason & USN_REASON_HARD_LINK_CHANGE)
          strcat_s(sz_reason, "\t\tUSN_REASON_HARD_LINK_CHANGE |\n");
        if(reason & USN_REASON_INDEXABLE_CHANGE)
          strcat_s(sz_reason, "\t\tUSN_REASON_INDEXABLE_CHANGE |\n");
        if(reason & USN_REASON_NAMED_DATA_EXTEND)
          strcat_s(sz_reason, "\t\tUSN_REASON_NAMED_DATA_EXTEND |\n");
        if(reason & USN_REASON_NAMED_DATA_OVERWRITE)
          strcat_s(sz_reason, "\t\tUSN_REASON_NAMED_DATA_OVERWRITE |\n");
        if(reason & USN_REASON_NAMED_DATA_TRUNCATION)
          strcat_s(sz_reason, "\t\tUSN_REASON_NAMED_DATA_TRUNCATION |\n");
        if(reason & USN_REASON_OBJECT_ID_CHANGE)
          strcat_s(sz_reason, "\t\tUSN_REASON_OBJECT_ID_CHANGE |\n");
        if(reason & USN_REASON_RENAME_NEW_NAME)
          strcat_s(sz_reason, "\t\tUSN_REASON_RENAME_NEW_NAME |\n");
        if(reason & USN_REASON_RENAME_OLD_NAME)
          strcat_s(sz_reason, "\t\tUSN_REASON_RENAME_OLD_NAME |\n");
        if(reason & USN_REASON_REPARSE_POINT_CHANGE)
          strcat_s(sz_reason, "\t\tUSN_REASON_REPARSE_POINT_CHANGE |\n");
        if(reason & USN_REASON_SECURITY_CHANGE)
          strcat_s(sz_reason, "\t\tUSN_REASON_SECURITY_CHANG |\n");
        if(reason & USN_REASON_STREAM_CHANGE)
          strcat_s(sz_reason, "\t\tUSN_REASON_STREAM_CHANG |\n");

        sz_reason[strlen(sz_reason) - 3] = 0;

        return rsl::string(sz_reason);
      }

      rsl::string parse_file_attribs(ulong attribs)
      {
        char8 sz_file_attribs[2048] = "";

        if(attribs & FILE_ATTRIBUTE_READONLY)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_READONLY |\n");
        if(attribs & FILE_ATTRIBUTE_HIDDEN)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_HIDDEN |\n");
        if(attribs & FILE_ATTRIBUTE_SYSTEM)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_SYSTEM |\n");
        if(attribs & FILE_ATTRIBUTE_DIRECTORY)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_DIRECTORY |\n");
        if(attribs & FILE_ATTRIBUTE_ARCHIVE)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_ARCHIVE |\n");
        if(attribs & FILE_ATTRIBUTE_DEVICE)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_DEVICE |\n");
        if(attribs & FILE_ATTRIBUTE_NORMAL)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_NORMAL |\n");
        if(attribs & FILE_ATTRIBUTE_TEMPORARY)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_TEMPORARY |\n");
        if(attribs & FILE_ATTRIBUTE_SPARSE_FILE)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_SPARSE_FILE |\n");
        if(attribs & FILE_ATTRIBUTE_REPARSE_POINT)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_REPARSE_POINT |\n");
        if(attribs & FILE_ATTRIBUTE_COMPRESSED)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_COMPRESSED |\n");
        if(attribs & FILE_ATTRIBUTE_OFFLINE)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_OFFLINE |\n");
        if(attribs & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_NOT_CONTENT_INDEXED |\n");
        if(attribs & FILE_ATTRIBUTE_ENCRYPTED)
          strcat_s(sz_file_attribs, "\t\tFILE_ATTRIBUTE_ENCRYPTED |\n");

        sz_file_attribs[strlen(sz_file_attribs) - 3] = 0;

        return rsl::string(sz_file_attribs);
      }

      FILE_ID_DESCRIPTOR get_file_id_desc(uint64 fileId)
      {
        FILE_ID_DESCRIPTOR file_desc;
        file_desc.Type            = FileIdType;
        file_desc.FileId.QuadPart = fileId;
        file_desc.dwSize          = sizeof(file_desc);

        return file_desc;
      }
      rsl::string get_filename(const rsl::win::handle& drive, uint64 fileReferenceNumber)
      {
        FILE_ID_DESCRIPTOR file_desc = get_file_id_desc(fileReferenceNumber);
        HANDLE handle                = OpenFileById(drive.get(), &file_desc, 0, 0, 0, 0);

        if(handle != INVALID_HANDLE_VALUE)
        {
          BY_HANDLE_FILE_INFORMATION info {};
          char buffer[256] {};
          GetFinalPathNameByHandle(handle, buffer, 256, 0);
          rsl::string res(buffer);

          rsl::string_view drive_prefix = "\\\\?\\";
          res.replace(0, drive_prefix.length(), "");
          return res;
        }

        return rsl::string("");
      }
    } // namespace ntfs
  }   // namespace win32
} // namespace rex