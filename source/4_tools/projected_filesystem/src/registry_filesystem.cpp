#include "projected_filesystem/registry_filesystem.h"

#include "rex_engine/platform/win/diagnostics/hr_call.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_engine/diagnostics/logging/log_macros.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/directory.h"

#include "rex_std/bonus/platform.h"

#pragma warning (push)
#pragma warning (disable: 4005) // Macro redefition
#include <ntstatus.h>   // For STATUS_CANNOT_DELETE
#pragma warning (pop)

#include <objbase.h>    // For CoCreateGuid

DEFINE_LOG_CATEGORY(LogProjFs);

namespace proj_fs
{
  namespace callbacks
  {

    HRESULT StartDirEnumCallback(
      const PRJ_CALLBACK_DATA* CallbackData,
      const GUID* EnumerationId
    )
    {
      auto instance = reinterpret_cast<RegistryFilesystem*>(CallbackData->InstanceContext);
      return instance->StartDirEnum(CallbackData, EnumerationId);
    }

    HRESULT EndDirEnumCallback(
      const PRJ_CALLBACK_DATA* CallbackData,
      const GUID* EnumerationId
    )
    {
      auto instance = reinterpret_cast<RegistryFilesystem*>(CallbackData->InstanceContext);
      return instance->EndDirEnum(CallbackData, EnumerationId);
    }

    HRESULT GetDirEnumCallback(
      const PRJ_CALLBACK_DATA* CallbackData,
      const GUID* EnumerationId,
      PCWSTR SearchExpression,
      PRJ_DIR_ENTRY_BUFFER_HANDLE DirEntryBufferHandle
    )
    {
      auto instance = reinterpret_cast<RegistryFilesystem*>(CallbackData->InstanceContext);
      return instance->GetDirEnum(CallbackData,
        EnumerationId,
        SearchExpression,
        DirEntryBufferHandle);
    }

    HRESULT GetPlaceholderInfoCallback(
      const PRJ_CALLBACK_DATA* CallbackData
    )
    {
      auto instance = reinterpret_cast<RegistryFilesystem*>(CallbackData->InstanceContext);
      return instance->GetPlaceholderInfo(CallbackData);
    }

    HRESULT GetFileDataCallback(
      const PRJ_CALLBACK_DATA* CallbackData,
      UINT64                      ByteOffset,
      UINT32                      Length
    )
    {
      auto instance = reinterpret_cast<RegistryFilesystem*>(CallbackData->InstanceContext);
      return instance->GetFileData(CallbackData, ByteOffset, Length);
    }

    HRESULT NotificationCallback(
      const PRJ_CALLBACK_DATA* CallbackData,
      BOOLEAN                         IsDirectory,
      PRJ_NOTIFICATION                NotificationType,
      PCWSTR                          DestinationFileName,
      PRJ_NOTIFICATION_PARAMETERS* NotificationParameters
    )
    {
      auto instance = reinterpret_cast<RegistryFilesystem*>(CallbackData->InstanceContext);
      return instance->Notify(CallbackData,
        IsDirectory,
        NotificationType,
        DestinationFileName,
        NotificationParameters);
    }

    HRESULT QueryFileNameCallback(
      const PRJ_CALLBACK_DATA* CallbackData)
    {
      auto instance = reinterpret_cast<RegistryFilesystem*>(CallbackData->InstanceContext);
      return instance->QueryFileName(CallbackData);
    }

    void CancelCommandCallback(
      const PRJ_CALLBACK_DATA* CallbackData)
    {
      auto instance = reinterpret_cast<RegistryFilesystem*>(CallbackData->InstanceContext);
      instance->CancelCommand(CallbackData);
    }
  }

  rsl::medium_stack_string create_filepath(rsl::string_view root, rsl::string_view dir)
  {
    rsl::medium_stack_string path = root;
    path += "/";
    for (wchar_t c : dir)
    {
      path += rsl::to_ascii(c);
    }

    return path;
  }

  rsl::string convert_physical_path_to_logical_path(rsl::string_view path, rsl::unordered_map<rsl::medium_stack_string, rsl::stack_string<char8, 3>>& physicalToLogicalMap)
  {
    rsl::string result;
    for (const auto& [physical, logical] : physicalToLogicalMap)
    {
      if (path.contains(physical))
      {
        result.append(logical);
        result.append(path.substr(physical.length()));
      }
    }

    return result;
  }

  RegistryFilesystem::RegistryFilesystem(rsl::string_view root)
    : m_root(root)
    , m_readonly_namespace(false)
    , m_callbacks()
  {
    // Verify if the root exists. If not try to create it
    if (!verify_root())
    {
      REX_ERROR(LogProjFs, "Failed to find root and failed to create it. Root: {}", root);
    }

    query_drives();

    // Specify the notifications that we want ProjFS to send to us.
    PRJ_NOTIFICATION_MAPPING notificationMappings[1] = {};
    notificationMappings[0].NotificationRoot = L"";
    notificationMappings[0].NotificationBitMask = PRJ_NOTIFY_FILE_OPENED |
      PRJ_NOTIFY_NEW_FILE_CREATED |
      PRJ_NOTIFY_FILE_OVERWRITTEN |
      PRJ_NOTIFY_PRE_RENAME |
      PRJ_NOTIFY_FILE_RENAMED |
      PRJ_NOTIFY_FILE_HANDLE_CLOSED_NO_MODIFICATION |
      PRJ_NOTIFY_FILE_HANDLE_CLOSED_FILE_MODIFIED |
      PRJ_NOTIFY_FILE_HANDLE_CLOSED_FILE_DELETED |
      PRJ_NOTIFY_PRE_DELETE;

    // Store the notification mapping we set up into a start options structure.  We leave all the
    // other options at their defaults.  
    PRJ_STARTVIRTUALIZING_OPTIONS opts = {};
    opts.NotificationMappings = notificationMappings;
    opts.NotificationMappingsCount = 1;
    opts.PoolThreadCount = 1;

    // Register the required callbacks.
    m_callbacks.StartDirectoryEnumerationCallback = callbacks::StartDirEnumCallback;
    m_callbacks.EndDirectoryEnumerationCallback = callbacks::EndDirEnumCallback;
    m_callbacks.GetDirectoryEnumerationCallback = callbacks::GetDirEnumCallback;
    m_callbacks.GetPlaceholderInfoCallback = callbacks::GetPlaceholderInfoCallback;
    m_callbacks.GetFileDataCallback = callbacks::GetFileDataCallback;
    m_callbacks.NotificationCallback = callbacks::NotificationCallback;
    //m_callbacks.QueryFileNameCallback             = callbacks::QueryFileNameCallback;
    //m_callbacks.CancelCommandCallback             = callbacks::CancelCommandCallback;

    rsl::wstring wide_root(m_root.data(), m_root.data() + m_root.length());

    HR_CALL(PrjStartVirtualizing(wide_root.c_str(),
      &m_callbacks,
      this,
      &opts,
      &m_instance_handle));
  }


  // Description:
  //
  //  ProjFS invokes this callback to request metadata information for a file or a directory.
  //
  //  If the file or directory exists in the provider's namespace, the provider calls
  //  PrjWritePlaceholderInfo() to give ProjFS the info for the requested name.
  //
  //  The metadata information ProjFS supports includes:
  //
  //      Mandatory:
  //          FileBasicInfo.IsDirectory - the requested name is a file or directory.
  //
  //      Mandatory for files:
  //          FileBasicInfo.FileSize - file size in bytes.
  //
  //      Optional:
  //          VersionInfo - A 256 bytes ID which can be used to distinguish different versions of file content
  //                        for one file name.
  //          FileBasicInfo.CreationTime/LastAccessTime/LastWriteTime/ChangeTime - timestamps of the file.
  //          FileBasicInfo.FileAttributes - File Attributes.
  //
  //      Optional and less commonly used:
  //          EaInformation - Extended attribute (EA) information.
  //          SecurityInformation - Security descriptor information.
  //          StreamsInformation - Alternate data stream information.
  //
  //  See also PRJ_PLACEHOLDER_INFORMATION structure in projectedfslib.h for more details.
  //
  //  If the file or directory doesn't exist in the provider's namespace, this callback returns
  //  HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND).
  //
  //  If the provider is unable to process the request (e.g. due to network error) or it wants to block
  //  the request, this callback returns an appropriate HRESULT error code.
  //
  //  Below is a list of example commands that demonstrate how GetPlaceholderInfo is called.
  //
  //  Assuming z:\reg doesn't exist, run 'regfs.exe z:\reg' to create the root.
  //  Now start another command line window, 'cd z:\reg' then run below commands in sequence.
  //
  //  1) cd HKEY_LOCAL_MACHINE
  //     The first time you cd into a folder that exists in provider's namespace, GetPlaceholderInfo is
  //     called with CallbackData->FilePathName = "HKEY_LOCAL_MACHINE".  This callback will cause an
  //     on-disk placeholder file called "HKEY_LOCAL_MACHINE" to be created under z:\reg.
  //
  //  2) cd .. & cd HKEY_LOCAL_MACHINE
  //     The second and subsequent time you cd into a folder that exists in provider's namespace, this
  //     callback will not be called because the on-disk placeholder for HKEY_LOCAL_MACHINE already exists.
  //
  //  3) mkdir newfolder
  //     If _readonlyNamespace is true, GetPlaceholderInfo returns ERROR_ACCESS_DENIED, so the mkdir command
  //     reports "Access is denied" and the placeholder is not created.  If _readonlyNamespace is false,
  //     GetPlaceholderInfo returns ERROR_FILE_NOT_FOUND so the command succeeds and newfolder is created.
  //
  //  4) cd SOFTWARE\Microsoft\.NETFramework
  //     The first time you cd into a deep path, GetPlaceholderInfo is called repeatedly with the
  //     following CallbackData->FilePathName values:
  //     1) "HKEY_LOCAL_MACHINE\SOFTWARE"
  //     2) "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft"
  //     3) "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\.NETFramework"

  HRESULT RegistryFilesystem::GetPlaceholderInfo(const PRJ_CALLBACK_DATA* CallbackData)
  {
    rsl::string filepath_name = rsl::to_string(rsl::wstring_view(CallbackData->FilePathName));
    rsl::medium_stack_string full_filepath = create_filepath(m_root, filepath_name);
    rsl::string process_image_name = rsl::to_string(rsl::wstring_view(CallbackData->TriggeringProcessImageFileName));
    process_image_name = convert_physical_path_to_logical_path(process_image_name, m_phyiscal_drive_to_logical_drive);

    REX_INFO(LogProjFs, "{} - LOADING PLACEHOLDER INFO FOR - \t\t{}", process_image_name, full_filepath);

    bool isKey;
    INT64 valSize = 0;

    // Find out whether the specified path exists in the registry, and whether it is a key or a value.
    std::wstring filepath = CallbackData->FilePathName;
    std::string filepath_ascii;
    filepath_ascii.reserve(filepath.size());
    for (const wchar_t c : filepath)
    {
      filepath_ascii.push_back((char)c);
    }
    if (m_reg_ops.DoesKeyExist(filepath_ascii))
    {
      isKey = true;
    }
    else if (m_reg_ops.DoesValueExist(filepath_ascii, valSize))
    {
      isKey = false;
    }
    else
    {
      wprintf(L"<---- %hs: return 0x%08x\n",
        __FUNCTION__, ERROR_FILE_NOT_FOUND);
      return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    PRJ_PLACEHOLDER_INFO place_holder_info = {};
    place_holder_info.FileBasicInfo.IsDirectory = isKey;
    place_holder_info.FileBasicInfo.FileSize = valSize;

    HRESULT hr = HR_CALL(PrjWritePlaceholderInfo(m_instance_handle, filepath.data(), &place_holder_info, sizeof(place_holder_info)));

    REX_INFO(LogProjFs, "<---- {}: return {:#x}, ", __FUNCTION__, hr);

    return hr;
  }

  // Description:
  // 
  //     ProjFS invokes this callback to tell the provider that a directory enumeration is starting.
  // 
  //     A user-mode tool usually uses FindFirstFile/FindNextFile APIs to enumerate a directory.  Those
  //     APIs send QueryDirectory requests to the file system.  If the enumeration is for a placeholder
  //     folder, ProjFS intercepts and blocks those requests.  Then ProjFS invokes the registered directory
  //     enumeration callbacks (StartDirEnum, GetDirEnum, EndDirEnum) to get a list of names in provider's
  //     namespace, merges those names with the names physically on disk under that folder, then unblocks
  //     the enumeration requests and returns the merged list to the caller.

  HRESULT RegistryFilesystem::StartDirEnum(const PRJ_CALLBACK_DATA* CallbackData, const GUID* EnumerationId)
  {
    rsl::string filepath_name = rsl::to_string(rsl::wstring_view(CallbackData->FilePathName));
    rsl::string process_image_name = CallbackData->TriggeringProcessImageFileName ? rsl::to_string(rsl::wstring_view(CallbackData->TriggeringProcessImageFileName)) : rsl::string("");
    process_image_name = convert_physical_path_to_logical_path(process_image_name, m_phyiscal_drive_to_logical_drive);
    REX_INFO(LogProjFs, "{} - STARTED DIR ENUM ON - \t\t{}", process_image_name, create_filepath(m_root, filepath_name));

    // For each dir enum session, ProjFS sends:
    //      one StartEnumCallback
    //      one or more GetEnumCallbacks
    //      one EndEnumCallback
    // These callbacks will use the same value for EnumerationId for the same session.
    // Here we map the EnumerationId to a new DirInfo object.
    m_enumeration_sessions.emplace(*EnumerationId, rsl::wstring_view(CallbackData->FilePathName));

    return S_OK;
  };

  // Description:
  // 
  //     ProjFS invokes this callback to tell the provider that a directory enumeration is over.  This
  //     gives the provider the opportunity to release any resources it set up for the enumeration.

  HRESULT RegistryFilesystem::EndDirEnum(const PRJ_CALLBACK_DATA* CallbackData, const GUID* EnumerationId)
  {
    rsl::string filepath_name = rsl::to_string(rsl::wstring_view(CallbackData->FilePathName));
    rsl::string process_image_name = CallbackData->TriggeringProcessImageFileName ? rsl::to_string(rsl::wstring_view(CallbackData->TriggeringProcessImageFileName)) : rsl::string("");
    process_image_name = convert_physical_path_to_logical_path(process_image_name, m_phyiscal_drive_to_logical_drive);
    REX_INFO(LogProjFs, "{} - ENDED DIR ENUM ON - \t\t{}", process_image_name, create_filepath(m_root, filepath_name));

    // Get rid of the DirInfo object we created in StartDirEnum.
    m_enumeration_sessions.erase(*EnumerationId);

    return S_OK;
  };

  // Description:
  // 
  //     ProjFS invokes this callback to request a list of files and directories under the given directory.
  // 
  //     To handle this callback, RegFS calls DirInfo->FillFileEntry/FillDirEntry for each matching file
  //     or directory.
  // 
  //     If the SearchExpression argument specifies something that doesn't exist in provider's namespace,
  //     or if the directory being enumerated is empty, the provider just returns S_OK without storing
  //     anything in DirEntryBufferHandle.  ProjFS will return the correct error code to the caller.
  // 
  //     Below is a list of example commands that will invoke GetDirectoryEntries callbacks.
  //     These assume you've cd'd into the virtualization root folder.
  // 
  //     Command                  CallbackData->FilePathName    SearchExpression
  //     ------------------------------------------------------------------------
  //     dir                      ""(root folder)               *
  //     dir foo*                 ""(root folder)               foo*
  //     dir H + TAB              ""(root folder)               H*
  //     dir HKEY_LOCAL_MACHINE   ""(root folder)               HKEY_LOCAL_MACHINE
  //     dir HKEY_LOCAL_MACHIN?   ""(root folder)               HKEY_LOCAL_MACHIN>
  // 
  //     In the last example, the ">" character is the special wildcard value DOS_QM.  ProjFS handles this
  //     and other special file system wildcard values in its PrjFileNameMatch and PrjDoesNameContainWildCards
  //     APIs.

  HRESULT RegistryFilesystem::GetDirEnum(
    const PRJ_CALLBACK_DATA* CallbackData,
    const GUID* EnumerationId,
    PCWSTR                      SearchExpression,
    PRJ_DIR_ENTRY_BUFFER_HANDLE DirEntryBufferHandle
  )
  {
    rsl::string filepath_name = rsl::to_string(rsl::wstring_view(CallbackData->FilePathName));
    rsl::string process_image_name = CallbackData->TriggeringProcessImageFileName ? rsl::to_string(rsl::wstring_view(CallbackData->TriggeringProcessImageFileName)) : rsl::string("");
    process_image_name = convert_physical_path_to_logical_path(process_image_name, m_phyiscal_drive_to_logical_drive);
    rsl::string search_expr = rsl::to_string(rsl::wstring_view(SearchExpression));

    REX_INFO(LogProjFs, "{} - GET DIR ENUM ON - \t\t{} [{}]", process_image_name, create_filepath(m_root, filepath_name), search_expr);

    HRESULT hr = S_OK;

    // Get the correct enumeration session from our map.
    auto it = m_enumeration_sessions.find(*EnumerationId);
    if (it == m_enumeration_sessions.end())
    {
      // We were asked for an enumeration we don't know about.
      hr = E_INVALIDARG;

      REX_INFO(LogProjFs, "<---- {}: Unknown enumeration ID", __FUNCTION__);

      return hr;
    }

    // Get out our DirInfo helper object, which manages the context for this enumeration.
    auto& dirInfo = it->value;

    // If the enumeration is restarting, reset our bookkeeping information.
    if (CallbackData->Flags & PRJ_CB_DATA_FLAG_ENUM_RESTART_SCAN)
    {
      dirInfo.Reset();
    }

    if (!dirInfo.EntriesFilled())
    {
      // The DirInfo associated with the current session hasn't been initialized yet.  This method
      // will enumerate the subkeys and values in the registry key corresponding to CallbackData->FilePathName.
      // For each one that matches SearchExpression it will create an entry to return to ProjFS
      // and store it in the DirInfo object.
      hr = PopulateDirInfoForPath(CallbackData->FilePathName, rsl::addressof(dirInfo), SearchExpression);

      if (FAILED(hr))
      {
        REX_INFO(LogProjFs, "<---- {}: Failed to populate dirInfo: {:#x}", __FUNCTION__, hr);
        return hr;
      }

      // This will ensure the entries in the DirInfo are sorted the way the file system expects.
      dirInfo.SortEntriesAndMarkFilled();
    }

    // Return our directory entries to ProjFS.
    while (dirInfo.CurrentIsValid())
    {
      // ProjFS allocates a fixed size buffer then invokes this callback.  The callback needs to
      // call PrjFillDirEntryBuffer to fill as many entries as possible until the buffer is full.
      auto basic_info = dirInfo.CurrentBasicInfo();
      if (S_OK != PrjFillDirEntryBuffer(dirInfo.CurrentFileName().data(),
        &basic_info,
        DirEntryBufferHandle))
      {
        break;
      }

      // Only move the current entry cursor after the entry was successfully filled, so that we
      // can start from the correct index in the next GetDirEnum callback for this enumeration
      // session.
      dirInfo.MoveNext();
    }

    return hr;
  };

  // Populates a DirInfo object with directory and file entires that represent the registry keys and
  // values that are under a given key.
  HRESULT RegistryFilesystem::PopulateDirInfoForPath(
    std::wstring                       relativePath,
    DirInfo* dirInfo,
    std::wstring                       searchExpression
  )
  {
    RegEntries entries;

    // Get a list of the registry keys and values under the given key.
    HRESULT hr = m_reg_ops.EnumerateKey(relativePath.c_str(), entries);
    if (FAILED(hr))
    {
      wprintf(L"%hs: Could not enumerate key: 0x%08x",
        __FUNCTION__, hr);
      return hr;
    }

    // Store each registry key that matches searchExpression as a directory entry.
    for (auto subKey : entries.SubKeys)
    {
      rsl::wstring wide_name(subKey.Name.cbegin(), subKey.Name.cend());
      if (PrjFileNameMatch(wide_name.c_str(), searchExpression.c_str()))
      {
        dirInfo->FillDirEntry(wide_name);
      }
    }

    // Store each registry value that matches searchExpression as a file entry.
    for (auto val : entries.Values)
    {
      rsl::wstring wide_name(val.Name.cbegin(), val.Name.cend());
      if (PrjFileNameMatch(wide_name.c_str(), searchExpression.c_str()))
      {
        dirInfo->FillFileEntry(wide_name, val.Size);
      }
    }

    return hr;
  }

  // Description:
  // 
  //     ProjFS invokes this callback to request the contents of a file.
  // 
  //     To handle this callback, the provider issues one or more calls to WriteFileData() to give
  //     ProjFS the file content. ProjFS will convert the on-disk placeholder into a hydrated placeholder,
  //     populated with the file contents.  Afterward, subsequent file reads will no longer invoke the
  //     GetFileStream callback.
  // 
  //     If multiple threads read the same placeholder file simultaneously, ProjFS ensures that the provider
  //     receives only one GetFileStream callback.
  // 
  //     If the provider is unable to process the request, it return an appropriate error code.  The caller
  //     who issued the read will receive an error, and the next file read for the same file will invoke
  //     GetFileStream again.
  // 
  //     Below is a list of example commands that will invoke GetFileStream callbacks.
  //     Assume there's a file named 'testfile' in provider's namespace:
  // 
  //     type testfile
  //     echo 123>>testfile
  //     echo 123>testfile

  HRESULT RegistryFilesystem::GetFileData(
    const PRJ_CALLBACK_DATA* CallbackData,
    UINT64                      ByteOffset,
    UINT32                      Length
  )
  {
    rsl::string filepath_name = rsl::to_string(rsl::wstring_view(CallbackData->FilePathName));
    rsl::string process_image_name = rsl::to_string(rsl::wstring_view(CallbackData->TriggeringProcessImageFileName));
    process_image_name = convert_physical_path_to_logical_path(process_image_name, m_phyiscal_drive_to_logical_drive);

    REX_INFO(LogProjFs, "{} - FILLING IN FILE DATA - \t\t{}", process_image_name, create_filepath(m_root, filepath_name));

    HRESULT hr = S_OK;

    // We're going to need alignment information that is stored in the instance to service this
    // callback.
    PRJ_VIRTUALIZATION_INSTANCE_INFO instanceInfo;
    hr = HR_CALL(PrjGetVirtualizationInstanceInfo(m_instance_handle, &instanceInfo));

    if (FAILED(hr))
    {
      REX_ERROR(LogProjFs, "<---- {}: PrjGetVirtualizationInstanceInfo: {:#x}", __FUNCTION__, hr);
      return hr;
    }

    // Allocate a buffer that adheres to the machine's memory alignment.  We have to do this in case
    // the caller who caused this callback to be invoked is performing non-cached I/O.  For more
    // details, see the topic "Providing File Data" in the ProjFS documentation.
    void* writeBuffer = nullptr;
    writeBuffer = PrjAllocateAlignedBuffer(m_instance_handle, Length);

    if (writeBuffer == nullptr)
    {
      REX_ERROR(LogProjFs, "<---- []: Could not allocate write buffer.", __FUNCTION__);
      return E_OUTOFMEMORY;
    }

    // Read the data out of the registry.
    if (!m_reg_ops.ReadValue(std::string(filepath_name.c_str()), reinterpret_cast<PBYTE>(writeBuffer), Length))
    {
      hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

      PrjFreeAlignedBuffer(writeBuffer);
      REX_ERROR(LogProjFs, "<---- %hs: Failed to read from registry.\n", __FUNCTION__);

      return hr;
    }

    // Call ProjFS to write the data we read into the on-disk placeholder.
    hr = PrjWriteFileData(m_instance_handle,
      &CallbackData->DataStreamId,
      reinterpret_cast<PVOID>(writeBuffer),
      ByteOffset,
      Length);

    if (FAILED(hr))
    {
      // If this callback returns an error, ProjFS will return this error code to the thread that
      // issued the file read, and the target file will remain an empty placeholder.
      REX_ERROR(LogProjFs, "{}: failed to write file for [{}]: {:#x}", __FUNCTION__, filepath_name, hr);
    }

    // Free the memory-aligned buffer we allocated.
    PrjFreeAlignedBuffer(writeBuffer);

    return hr;
  }

  // Description:
  // 
  //     ProjFS invokes this callback to deliver notifications of file system operations.
  // 
  //     The provider can specify which notifications it wishes to receive by filling out an array of
  //     PRJ_NOTIFICATION_MAPPING structures that it feeds to PrjStartVirtualizing in the PRJ_STARTVIRTUALIZING_OPTIONS
  //     structure.
  // 
  //     For the following notifications the provider can return a failure code.  This will prevent the
  //     associated file system operation from taking place.
  // 
  //     PRJ_NOTIFICATION_FILE_OPENED
  //     PRJ_NOTIFICATION_PRE_DELETE
  //     PRJ_NOTIFICATION_PRE_RENAME
  //     PRJ_NOTIFICATION_PRE_SET_HARDLINK
  //     PRJ_NOTIFICATION_FILE_PRE_CONVERT_TO_FULL
  // 
  //     All other notifications are informational only.
  // 
  //     See also the PRJ_NOTIFICATION_TYPE enum for more details about the notification types.

  HRESULT RegistryFilesystem::Notify(
    const PRJ_CALLBACK_DATA* CallbackData,
    BOOLEAN                         /*IsDirectory*/,
    PRJ_NOTIFICATION                NotificationType,
    PCWSTR                          DestinationFileName,
    PRJ_NOTIFICATION_PARAMETERS* /*NotificationParameters*/
  )
  {
    rsl::string filepath_name = rsl::to_string(rsl::wstring_view(CallbackData->FilePathName));
    rsl::medium_stack_string full_filepath = create_filepath(m_root, filepath_name);
    rsl::string process_image_name = rsl::to_string(rsl::wstring_view(CallbackData->TriggeringProcessImageFileName));
    process_image_name = convert_physical_path_to_logical_path(process_image_name, m_phyiscal_drive_to_logical_drive);

    HRESULT hr = S_OK;

    switch (NotificationType)
    {
    case PRJ_NOTIFICATION_FILE_OPENED: notify_file_opened(process_image_name, full_filepath); break;
    case PRJ_NOTIFICATION_FILE_HANDLE_CLOSED_FILE_MODIFIED: notify_file_overwritten(process_image_name, full_filepath); break;
    case PRJ_NOTIFICATION_FILE_OVERWRITTEN: notify_file_overwritten(process_image_name, full_filepath); break;
    case PRJ_NOTIFICATION_NEW_FILE_CREATED: notify_file_created(process_image_name, full_filepath); break;
    case PRJ_NOTIFICATION_FILE_RENAMED:
    {
      rsl::string destination = rsl::to_string(rsl::wstring_view(DestinationFileName));
      notify_renamed(process_image_name, full_filepath, create_filepath(m_root, destination));
      break;

    }
    case PRJ_NOTIFICATION_FILE_HANDLE_CLOSED_FILE_DELETED: notify_closed_with_deletion(process_image_name, full_filepath); break;
    case PRJ_NOTIFICATION_PRE_RENAME:
      if (m_readonly_namespace)
      {
        // Block file renames.
        hr = HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
        REX_INFO(LogProjFs, "----- rename request for [{}] was rejected ", create_filepath(m_root, filepath_name));
      }
      else
      {
        rsl::string destination = rsl::to_string(rsl::wstring_view(DestinationFileName));
        notify_pre_rename(process_image_name, full_filepath, create_filepath(m_root, destination));
      }
      break;
    case PRJ_NOTIFICATION_PRE_DELETE:
      if (m_readonly_namespace)
      {
        // Block file deletion.  We must return a particular NTSTATUS to ensure the file system
        // properly recognizes that this is a deny-delete.
        hr = HRESULT_FROM_NT(STATUS_CANNOT_DELETE);
        REX_INFO(LogProjFs, "----- delete request for [{}] was rejected ", create_filepath(m_root, filepath_name));
      }
      else
      {
        notify_pre_delete(process_image_name, full_filepath);
      }
      break;

    case PRJ_NOTIFICATION_PRE_SET_HARDLINK:
    {
      rsl::string destination = rsl::to_string(rsl::wstring_view(DestinationFileName));
      notify_pre_set_hardlink(full_filepath, create_filepath(m_root, destination));
      break;
    }
    case PRJ_NOTIFICATION_HARDLINK_CREATED:
    {
      rsl::string destination = rsl::to_string(rsl::wstring_view(DestinationFileName));
      notify_hardlink_created(full_filepath, create_filepath(m_root, destination));
      break;
    }
    case PRJ_NOTIFICATION_FILE_HANDLE_CLOSED_NO_MODIFICATION: notify_closed_no_modification(process_image_name, full_filepath); break;
    case PRJ_NOTIFICATION_FILE_PRE_CONVERT_TO_FULL: notify_pre_hydration(full_filepath); break;
    
    default: REX_INFO(LogProjFs, "%hs: Unexpected notification", __FUNCTION__); break;
    }

    //REX_INFO(LogProjFs, "<---- {}: return {:#x}", __FUNCTION__, hr);
    return hr;
  }

  bool RegistryFilesystem::verify_root()
  {
    if (rex::directory::exists(m_root))
    {
      rex::vfs::set_root(m_root);
      return true;
    }

    if (!rex::vfs::create_dir(m_root))
    {
      return false;
    }

    rex::vfs::set_root(m_root);

    GUID instance_id;
    CoCreateGuid(&instance_id);

    if (!rex::vfs::write_to_file("virt_root.txt", &instance_id, sizeof(instance_id), rex::vfs::AppendToFile::no))
    {
      return false;
    }

    rsl::wstring wide_root = rsl::to_wstring(m_root);
    // Mark the directory as the virtualization root.
    auto hr = PrjMarkDirectoryAsPlaceholder(wide_root.c_str(),
      nullptr,
      nullptr,
      &instance_id);

    return SUCCEEDED(hr);
  }

  void RegistryFilesystem::query_drives()
  {
    rsl::array<char8, 128> drives_buffer;
    DWORD bytes_written = WIN_CALL(GetLogicalDriveStringsA(drives_buffer.max_size(), drives_buffer.data()));
    constexpr card32 single_string_length = 4;

    rsl::vector<rsl::stack_string<char8, 3>> logical_drives(rsl::Capacity(bytes_written / single_string_length));
    for (DWORD i = 0; i < bytes_written; i += single_string_length)
    {
      logical_drives.emplace_back(&drives_buffer[i], 2); // we only care about the drive letter and the colon. eg. C:
    }

    rsl::medium_stack_string physical_drive;
    for (const auto drive : logical_drives)
    {
      WIN_CALL(QueryDosDevice(drive.data(), physical_drive.data(), physical_drive.max_size()));
      physical_drive.reset_null_termination_offset();

      m_phyiscal_drive_to_logical_drive[physical_drive] = drive;
    }
  }

  HRESULT RegistryFilesystem::QueryFileName(
    const PRJ_CALLBACK_DATA* /*CallbackData*/
  )
  {
    return S_OK;
  }

  void RegistryFilesystem::CancelCommand(
    const PRJ_CALLBACK_DATA* /*CallbackData*/
  )
  {

  }

  void RegistryFilesystem::notify_file_opened(rsl::string_view app, rsl::string_view path)
  {
    REX_UNUSED_PARAM(app);
    REX_UNUSED_PARAM(path);

    REX_INFO(LogProjFs, "{} - OPENED - \t\t\t\t{}", app, path);
  }
  void RegistryFilesystem::notify_file_created(rsl::string_view app, rsl::string_view path)
  {
    REX_UNUSED_PARAM(app);
    REX_UNUSED_PARAM(path);

    REX_INFO(LogProjFs, "{} - CLOSED - \t\t{}", app, path);
  }
  void RegistryFilesystem::notify_file_overwritten(rsl::string_view app, rsl::string_view path)
  {
    REX_UNUSED_PARAM(app);
    REX_UNUSED_PARAM(path);

    REX_INFO(LogProjFs, "{} - MODIFIED - \t\t{}", app, path);
  }
  void RegistryFilesystem::notify_pre_delete(rsl::string_view app, rsl::string_view path)
  {
    REX_UNUSED_PARAM(app);
    REX_UNUSED_PARAM(path);

    REX_INFO(LogProjFs, "{} - TRIES TO DELETE - \t\t{}", app, path);
  }
  void RegistryFilesystem::notify_pre_rename(rsl::string_view app, rsl::string_view oldPath, rsl::string_view newPath)
  {
    REX_UNUSED_PARAM(app);
    REX_UNUSED_PARAM(oldPath);
    REX_UNUSED_PARAM(newPath);

    REX_INFO(LogProjFs, "{} - TRIES TO RENAME - \t\t{} to {}", app, oldPath, newPath);
  }
  void RegistryFilesystem::notify_pre_set_hardlink(rsl::string_view newHardlink, rsl::string_view hardLinkTarget)
  {
    REX_UNUSED_PARAM(newHardlink);
    REX_UNUSED_PARAM(hardLinkTarget);

    REX_INFO(LogProjFs, "TRYING TO HARDLINK FROM - \t\t{} TO {}", newHardlink, hardLinkTarget);
  }
  void RegistryFilesystem::notify_renamed(rsl::string_view app, rsl::string_view oldPath, rsl::string_view newPath)
  {
    REX_UNUSED_PARAM(app);
    REX_UNUSED_PARAM(oldPath);
    REX_UNUSED_PARAM(newPath);

    REX_INFO(LogProjFs, "{} - RENAMED - \t\t{} TO {}", app, oldPath, newPath);
  }
  void RegistryFilesystem::notify_hardlink_created(rsl::string_view newHardlink, rsl::string_view hardLinkTarget)
  {
    REX_UNUSED_PARAM(newHardlink);
    REX_UNUSED_PARAM(hardLinkTarget);

    REX_INFO(LogProjFs, "HARDLINK CREATED FROM - \t\t{} TO {}", newHardlink, hardLinkTarget);
  }
  void RegistryFilesystem::notify_closed_no_modification(rsl::string_view app, rsl::string_view path)
  {
    REX_UNUSED_PARAM(app);
    REX_UNUSED_PARAM(path);

    REX_INFO(LogProjFs, "{} - CLOSED NO MODIFICATION - \t\t{}", app, path);

  }
  void RegistryFilesystem::notify_closed_with_modification(rsl::string_view app, rsl::string_view path)
  {
    REX_UNUSED_PARAM(app);
    REX_UNUSED_PARAM(path);

    REX_INFO(LogProjFs, "{} - CLOSED WITH MODIFICATION - \t\t{}", app, path);
  }
  void RegistryFilesystem::notify_closed_with_deletion(rsl::string_view app, rsl::string_view path)
  {
    REX_UNUSED_PARAM(app);
    REX_UNUSED_PARAM(path);

    REX_INFO(LogProjFs, "{} - CLOSED WITH DELETION - \t\t{}", app, path);
  }
  void RegistryFilesystem::notify_pre_hydration(rsl::string_view path)
  {
    REX_UNUSED_PARAM(path);

    REX_INFO(LogProjFs, "HYDRATED {}", path);
  }
}