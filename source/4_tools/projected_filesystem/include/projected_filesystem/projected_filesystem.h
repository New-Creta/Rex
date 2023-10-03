#pragma once

#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"
#include "rex_std/string.h"
#include "rex_std/functional.h"
#include "rex_std/set.h"

#include "projected_filesystem/dirInfo.h"

#include <windows.h>
#include <objbase.h>    // For CoCreateGuid

#include <projectedfslib.h>

namespace rsl
{
  inline namespace v1
  {
    template <>
    struct hash<GUID>
    {
      hash_result operator()(const GUID& guid) const
      {
        static_assert(sizeof(GUID) / sizeof(uint32) == 4, "Unexpected size of GUID");

        hash_result seed = 0;
        const int32* p = reinterpret_cast<const int32*>(&guid);
        for (card32 i = 0; i < 4; ++i)
        {
          seed = rsl::internal::hash_combine(seed, rsl::hash<int32>{}(*p));
          ++p;
        }

        return seed;
      }
    };
  }
}

namespace proj_fs
{
  class ProjectedFilesystem
  {
  public:
    ProjectedFilesystem(rsl::string_view root);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Virtualization instance callbacks (kernel mode -> user mode).  These are the methods the derived
    // provider class overrides.
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // [Mandatory] Inform the provider a directory enumeration is starting.
    // It corresponds to PRJ_START_DIRECTORY_ENUMERATION_CB in projectedfslib.h
    HRESULT StartDirEnum(
      const PRJ_CALLBACK_DATA* CallbackData,
      const GUID* EnumerationId
    );

    // [Mandatory] Inform the provider a directory enumeration is over.
    // It corresponds to PRJ_END_DIRECTORY_ENUMERATION_CB in projectedfslib.h
    HRESULT EndDirEnum(
      const PRJ_CALLBACK_DATA* CallbackData,
      const GUID* EnumerationId
    );

    // [Mandatory] Request directory enumeration information from the provider.
    // It corresponds to PRJ_GET_DIRECTORY_ENUMERATION_CB in projectedfslib.h
    HRESULT GetDirEnum(
      const PRJ_CALLBACK_DATA* CallbackData,
      const GUID* EnumerationId,
      PCWSTR                      SearchExpression,
      PRJ_DIR_ENTRY_BUFFER_HANDLE DirEntryBufferHandle
    );

    // [Mandatory] Request meta data information for a file or directory.
    // It corresponds to PRJ_GET_PLACEHOLDER_INFO_CB in projectedfslib.h
    HRESULT GetPlaceholderInfo(
      const PRJ_CALLBACK_DATA* CallbackData
    );

    // [Mandatory] Request the contents of a file's primary data stream.
    // It corresponds to PRJ_GET_FILE_DATA_CB in projectedfslib.h
    HRESULT GetFileData(
      const PRJ_CALLBACK_DATA* CallbackData,
      UINT64                      ByteOffset,
      UINT32                      Length
    );

    // [Optional] Deliver notifications to the provider that it has specified 
    // it wishes to receive. It corresponds to PRJ_NOTIFICATION_CB in projectedfslib.h
    HRESULT Notify(
      const PRJ_CALLBACK_DATA* CallbackData,
      BOOLEAN                         IsDirectory,
      PRJ_NOTIFICATION                NotificationType,
      PCWSTR                          DestinationFileName,
      PRJ_NOTIFICATION_PARAMETERS* NotificationParameters
    );

    HRESULT QueryFileName(
      const PRJ_CALLBACK_DATA* CallbackData
    );

    void CancelCommand(
      const PRJ_CALLBACK_DATA* CallbackData
    );

  private:
    bool verify_root();
    void query_drives();

    void notify_file_opened(rsl::string_view app, rsl::string_view path);
    void notify_file_created(rsl::string_view app, rsl::string_view path);
    void notify_file_overwritten(rsl::string_view app, rsl::string_view path);
    void notify_pre_delete(rsl::string_view app, rsl::string_view path);
    void notify_pre_rename(rsl::string_view app, rsl::string_view oldPath, rsl::string_view newPath);
    void notify_pre_set_hardlink(rsl::string_view newHardlink, rsl::string_view hardLinkTarget);
    void notify_renamed(rsl::string_view app, rsl::string_view oldPath, rsl::string_view newPath);
    void notify_hardlink_created(rsl::string_view newHardlink, rsl::string_view hardLinkTarget);
    void notify_closed_no_modification(rsl::string_view app, rsl::string_view path);
    void notify_closed_with_modification(rsl::string_view app, rsl::string_view path);
    void notify_closed_with_deletion(rsl::string_view app, rsl::string_view path);
    void notify_pre_hydration(rsl::string_view path);

    void break_hardlink(rsl::string_view path);

    HRESULT PopulateDirInfoForPath(
      std::wstring                       relativePath,
      DirInfo* dirInfo,
      std::wstring                       searchExpression);

  private:
    PRJ_CALLBACKS m_callbacks = {};
    PRJ_NAMESPACE_VIRTUALIZATION_CONTEXT m_instance_handle = nullptr;
    rsl::medium_stack_string m_root;
    rsl::unordered_map<GUID, DirInfo> m_enumeration_sessions;
    rsl::unordered_map<rsl::medium_stack_string, rsl::stack_string<char8, 3>> m_phyiscal_drive_to_logical_drive;
    rsl::set<rsl::medium_stack_string> m_writeable_files;
    bool m_readonly_namespace;
  };
}