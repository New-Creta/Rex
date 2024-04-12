#include "dokan_fs/internal/dokan/dokan.h"
#include "dokan_fs/internal/dokan/fileinfo.h"
#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/event_system/event_system.h"
#include "rex_std/bonus/memory.h"
#include "rex_windows/engine/platform_creation_params.h"

#include <Windows.h>

// Enable Long Paths on Windows 10 version 1607 and later by changing
// the OS configuration (see Microsoft own documentation for the steps)
// and rebuild the mirror with the following line uncommented.

// #define WIN10_ENABLE_LONG_PATH
#ifdef WIN10_ENABLE_LONG_PATH
  // dirty but should be enough
  #define DOKAN_MAX_PATH 32768
#else
  #define DOKAN_MAX_PATH MAX_PATH
#endif // DEBUG

BOOL g_UseStdErr;
BOOL g_DebugMode;
BOOL g_CaseSensitive;
BOOL g_HasSeSecurityPrivilege;
BOOL g_ImpersonateCallerUser;


rsl::wstring g_blob_store_path(L"C:");
rsl::wstring g_mount_point(L"M:\\");
rsl::wstring g_unc_name(L"");
rsl::wstring g_volume_name(L"REX DOKAN");

void GetFilePath(PWCHAR filePath, ULONG numberOfElements, LPCWSTR FileName)
{
  wcsncpy_s(filePath, numberOfElements, g_blob_store_path.data(), g_blob_store_path.length());
  wcsncat_s(filePath, numberOfElements, FileName, wcslen(FileName));
}

void PrintUserName(PDOKAN_FILE_INFO DokanFileInfo)
{
  HANDLE handle;
  UCHAR buffer[1024];
  DWORD returnLength;
  WCHAR accountName[256];
  WCHAR domainName[256];
  DWORD accountLength = sizeof(accountName) / sizeof(WCHAR);
  DWORD domainLength  = sizeof(domainName) / sizeof(WCHAR);
  PTOKEN_USER tokenUser;
  SID_NAME_USE snu;

  if(!g_DebugMode)
    return;

  handle = DokanOpenRequestorToken(DokanFileInfo);
  if(handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "  DokanOpenRequestorToken failed");
    return;
  }

  if(!GetTokenInformation(handle, TokenUser, buffer, sizeof(buffer), &returnLength))
  {
    REX_ERROR(LogDokan, "  GetTokenInformaiton failed: {}", GetLastError());
    CloseHandle(handle);
    return;
  }

  CloseHandle(handle);

  tokenUser = (PTOKEN_USER)buffer;
  if(!LookupAccountSidW(NULL, tokenUser->User.Sid, accountName, &accountLength, domainName, &domainLength, &snu))
  {
    REX_ERROR(LogDokan, "  LookupAccountSid failed: {}", GetLastError());
    return;
  }

  REX_INFO(LogDokan, L"  AccountName: {}, DomainName: {}", accountName, domainName);
}

BOOL AddSeSecurityNamePrivilege()
{
  HANDLE token = 0;
  REX_INFO(LogDokan, "## Attempting to add SE_SECURITY_NAME privilege to process token ##");
  DWORD err;
  LUID luid;
  if(!LookupPrivilegeValue(0, SE_SECURITY_NAME, &luid))
  {
    err = GetLastError();
    if(err != ERROR_SUCCESS)
    {
      REX_ERROR(LogDokan, "  failed: Unable to lookup privilege value. error = {}", err);
      return FALSE;
    }
  }

  LUID_AND_ATTRIBUTES attr;
  attr.Attributes = SE_PRIVILEGE_ENABLED;
  attr.Luid       = luid;

  TOKEN_PRIVILEGES priv;
  priv.PrivilegeCount = 1;
  priv.Privileges[0]  = attr;

  if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
  {
    err = GetLastError();
    if(err != ERROR_SUCCESS)
    {
      REX_ERROR(LogDokan, "  failed: Unable obtain process token. error = {}", err);
      return FALSE;
    }
  }

  TOKEN_PRIVILEGES oldPriv;
  DWORD retSize;
  AdjustTokenPrivileges(token, FALSE, &priv, sizeof(TOKEN_PRIVILEGES), &oldPriv, &retSize);
  err = GetLastError();
  if(err != ERROR_SUCCESS)
  {
    REX_ERROR(LogDokan, "  failed: Unable to adjust token privileges: {}", err);
    CloseHandle(token);
    return FALSE;
  }

  BOOL privAlreadyPresent = FALSE;
  for(unsigned int i = 0; i < oldPriv.PrivilegeCount; i++)
  {
    if(oldPriv.Privileges[i].Luid.HighPart == luid.HighPart && oldPriv.Privileges[i].Luid.LowPart == luid.LowPart)
    {
      privAlreadyPresent = TRUE;
      break;
    }
  }
  REX_ERROR(LogDokan, privAlreadyPresent ? "  success: privilege already present" : "  success: privilege added");
  if(token)
    CloseHandle(token);
  return TRUE;
}

#define MirrorCheckFlag(val, flag)                                                                                                                                                                                                                       \
  if(val & flag)                                                                                                                                                                                                                                         \
  {                                                                                                                                                                                                                                                      \
    REX_INFO(LogDokan, "\t" #flag);                                                                                                                                                                                                                     \
  }

NTSTATUS DOKAN_CALLBACK MirrorCreateFile(LPCWSTR FileName, PDOKAN_IO_SECURITY_CONTEXT SecurityContext, ACCESS_MASK DesiredAccess, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions,
                                                PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle;
  DWORD fileAttr;
  NTSTATUS status = STATUS_SUCCESS;
  DWORD creationDisposition;
  DWORD fileAttributesAndFlags;
  DWORD error = 0;
  SECURITY_ATTRIBUTES securityAttrib;
  ACCESS_MASK genericDesiredAccess;
  // userTokenHandle is for Impersonate Caller User Option
  HANDLE userTokenHandle = INVALID_HANDLE_VALUE;

  securityAttrib.nLength              = sizeof(securityAttrib);
  securityAttrib.lpSecurityDescriptor = SecurityContext->AccessState.SecurityDescriptor;
  securityAttrib.bInheritHandle       = FALSE;

  DokanMapKernelToUserCreateFileFlags(DesiredAccess, FileAttributes, CreateOptions, CreateDisposition, &genericDesiredAccess, &fileAttributesAndFlags, &creationDisposition);

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"CreateFileW : {}", filePath);

  PrintUserName(DokanFileInfo);

  /*
  if (ShareMode == 0 && AccessMode & FILE_WRITE_DATA)
          ShareMode = FILE_SHARE_WRITE;
  else if (ShareMode == 0)
          ShareMode = FILE_SHARE_READ;
  */

  REX_INFO(LogDokan, "\tShareMode = {:#x}", ShareAccess);

  MirrorCheckFlag(ShareAccess, FILE_SHARE_READ);
  MirrorCheckFlag(ShareAccess, FILE_SHARE_WRITE);
  MirrorCheckFlag(ShareAccess, FILE_SHARE_DELETE);

  REX_INFO(LogDokan, "\tDesiredAccess = {:#x}", DesiredAccess);

  MirrorCheckFlag(DesiredAccess, GENERIC_READ);
  MirrorCheckFlag(DesiredAccess, GENERIC_WRITE);
  MirrorCheckFlag(DesiredAccess, GENERIC_EXECUTE);

  MirrorCheckFlag(DesiredAccess, DELETE);
  MirrorCheckFlag(DesiredAccess, FILE_READ_DATA);
  MirrorCheckFlag(DesiredAccess, FILE_READ_ATTRIBUTES);
  MirrorCheckFlag(DesiredAccess, FILE_READ_EA);
  MirrorCheckFlag(DesiredAccess, READ_CONTROL);
  MirrorCheckFlag(DesiredAccess, FILE_WRITE_DATA);
  MirrorCheckFlag(DesiredAccess, FILE_WRITE_ATTRIBUTES);
  MirrorCheckFlag(DesiredAccess, FILE_WRITE_EA);
  MirrorCheckFlag(DesiredAccess, FILE_APPEND_DATA);
  MirrorCheckFlag(DesiredAccess, WRITE_DAC);
  MirrorCheckFlag(DesiredAccess, WRITE_OWNER);
  MirrorCheckFlag(DesiredAccess, SYNCHRONIZE);
  MirrorCheckFlag(DesiredAccess, FILE_EXECUTE);
  MirrorCheckFlag(DesiredAccess, STANDARD_RIGHTS_READ);
  MirrorCheckFlag(DesiredAccess, STANDARD_RIGHTS_WRITE);
  MirrorCheckFlag(DesiredAccess, STANDARD_RIGHTS_EXECUTE);

  // When filePath is a directory, needs to change the flag so that the file can
  // be opened.
  fileAttr = GetFileAttributesW(filePath);

  if(fileAttr != INVALID_FILE_ATTRIBUTES && fileAttr & FILE_ATTRIBUTE_DIRECTORY)
  {
    if(CreateOptions & FILE_NON_DIRECTORY_FILE)
    {
      REX_ERROR(LogDokan, "\tCannot open a dir as a file");
      return STATUS_FILE_IS_A_DIRECTORY;
    }
    DokanFileInfo->IsDirectory = TRUE;
    // Needed by FindFirstFileW to list files in it
    // TODO: use ReOpenFile in MirrorFindFiles to set share read temporary
    ShareAccess |= FILE_SHARE_READ;
  }

  REX_INFO(LogDokan, "\tFlagsAndAttributes = {:#x}", fileAttributesAndFlags);

  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_ARCHIVE);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_COMPRESSED);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_DEVICE);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_DIRECTORY);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_ENCRYPTED);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_HIDDEN);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_INTEGRITY_STREAM);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_NORMAL);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_NO_SCRUB_DATA);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_OFFLINE);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_READONLY);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_REPARSE_POINT);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_SPARSE_FILE);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_SYSTEM);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_TEMPORARY);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_ATTRIBUTE_VIRTUAL);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_FLAG_WRITE_THROUGH);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_FLAG_OVERLAPPED);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_FLAG_NO_BUFFERING);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_FLAG_RANDOM_ACCESS);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_FLAG_SEQUENTIAL_SCAN);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_FLAG_DELETE_ON_CLOSE);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_FLAG_BACKUP_SEMANTICS);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_FLAG_POSIX_SEMANTICS);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_FLAG_OPEN_REPARSE_POINT);
  MirrorCheckFlag(fileAttributesAndFlags, FILE_FLAG_OPEN_NO_RECALL);
  MirrorCheckFlag(fileAttributesAndFlags, SECURITY_ANONYMOUS);
  MirrorCheckFlag(fileAttributesAndFlags, SECURITY_IDENTIFICATION);
  MirrorCheckFlag(fileAttributesAndFlags, SECURITY_IMPERSONATION);
  MirrorCheckFlag(fileAttributesAndFlags, SECURITY_DELEGATION);
  MirrorCheckFlag(fileAttributesAndFlags, SECURITY_CONTEXT_TRACKING);
  MirrorCheckFlag(fileAttributesAndFlags, SECURITY_EFFECTIVE_ONLY);
  MirrorCheckFlag(fileAttributesAndFlags, SECURITY_SQOS_PRESENT);

  if(g_CaseSensitive)
    fileAttributesAndFlags |= FILE_FLAG_POSIX_SEMANTICS;

  if(creationDisposition == CREATE_NEW)
  {
    REX_INFO(LogDokan, "\tCREATE_NEW");
  }
  else if(creationDisposition == OPEN_ALWAYS)
  {
    REX_INFO(LogDokan, "\tOPEN_ALWAYS");
  }
  else if(creationDisposition == CREATE_ALWAYS)
  {
    REX_INFO(LogDokan, "\tCREATE_ALWAYS");
  }
  else if(creationDisposition == OPEN_EXISTING)
  {
    REX_INFO(LogDokan, "\tOPEN_EXISTING");
  }
  else if(creationDisposition == TRUNCATE_EXISTING)
  {
    REX_INFO(LogDokan, "\tTRUNCATE_EXISTING");
  }
  else
  {
    REX_WARN(LogDokan, "\tUNKNOWN creationDisposition!");
  }

  if(g_ImpersonateCallerUser)
  {
    userTokenHandle = DokanOpenRequestorToken(DokanFileInfo);

    if(userTokenHandle == INVALID_HANDLE_VALUE)
    {
      REX_ERROR(LogDokan, "  DokanOpenRequestorToken failed");
      // Should we return some error?
    }
  }

  if(DokanFileInfo->IsDirectory)
  {
    // It is a create directory request

    if(creationDisposition == CREATE_NEW || creationDisposition == OPEN_ALWAYS)
    {
      if(g_ImpersonateCallerUser && userTokenHandle != INVALID_HANDLE_VALUE)
      {
        // if g_ImpersonateCallerUser option is on, call the ImpersonateLoggedOnUser function.
        if(!ImpersonateLoggedOnUser(userTokenHandle))
        {
          // handle the error if failed to impersonate
          REX_ERROR(LogDokan, "\tImpersonateLoggedOnUser failed.");
        }
      }

      // We create folder
      if(!CreateDirectoryW(filePath, &securityAttrib))
      {
        error = GetLastError();
        // Fail to create folder for OPEN_ALWAYS is not an error
        if(error != ERROR_ALREADY_EXISTS || creationDisposition == CREATE_NEW)
        {
          REX_ERROR(LogDokan, "\terror code = {}", error);
          status = DokanNtStatusFromWin32(error);
        }
      }

      if(g_ImpersonateCallerUser && userTokenHandle != INVALID_HANDLE_VALUE)
      {
        // Clean Up operation for impersonate
        DWORD lastError = GetLastError();
        if(status != STATUS_SUCCESS) // Keep the handle open for CreateFileW
          CloseHandle(userTokenHandle);
        RevertToSelf();
        SetLastError(lastError);
      }
    }

    if(status == STATUS_SUCCESS)
    {
      // Check first if we're trying to open a file as a directory.
      if(fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY) && (CreateOptions & FILE_DIRECTORY_FILE))
      {
        return STATUS_NOT_A_DIRECTORY;
      }

      if(g_ImpersonateCallerUser && userTokenHandle != INVALID_HANDLE_VALUE)
      {
        // if g_ImpersonateCallerUser option is on, call the ImpersonateLoggedOnUser function.
        if(!ImpersonateLoggedOnUser(userTokenHandle))
        {
          // handle the error if failed to impersonate
          REX_ERROR(LogDokan, "\tImpersonateLoggedOnUser failed.");
        }
      }

      // FILE_FLAG_BACKUP_SEMANTICS is required for opening directory handles
      handle = CreateFileW(filePath, genericDesiredAccess, ShareAccess, &securityAttrib, OPEN_EXISTING, fileAttributesAndFlags | FILE_FLAG_BACKUP_SEMANTICS, NULL);

      if(g_ImpersonateCallerUser && userTokenHandle != INVALID_HANDLE_VALUE)
      {
        // Clean Up operation for impersonate
        DWORD lastError = GetLastError();
        CloseHandle(userTokenHandle);
        RevertToSelf();
        SetLastError(lastError);
      }

      if(handle == INVALID_HANDLE_VALUE)
      {
        error = GetLastError();
        REX_ERROR(LogDokan, "\terror code = {}", error);

        status = DokanNtStatusFromWin32(error);
      }
      else
      {
        DokanFileInfo->Context = (ULONG64)handle; // save the file handle in Context

        // Open succeed but we need to inform the driver
        // that the dir open and not created by returning STATUS_OBJECT_NAME_COLLISION
        if(creationDisposition == OPEN_ALWAYS && fileAttr != INVALID_FILE_ATTRIBUTES)
          return STATUS_OBJECT_NAME_COLLISION;
      }
    }
  }
  else
  {
    // It is a create file request

    // Cannot overwrite a hidden or system file if flag not set
    if(fileAttr != INVALID_FILE_ATTRIBUTES && ((!(fileAttributesAndFlags & FILE_ATTRIBUTE_HIDDEN) && (fileAttr & FILE_ATTRIBUTE_HIDDEN)) || (!(fileAttributesAndFlags & FILE_ATTRIBUTE_SYSTEM) && (fileAttr & FILE_ATTRIBUTE_SYSTEM))) &&
       (creationDisposition == TRUNCATE_EXISTING || creationDisposition == CREATE_ALWAYS))
      return STATUS_ACCESS_DENIED;

    // Cannot delete a read only file
    if((fileAttr != INVALID_FILE_ATTRIBUTES && (fileAttr & FILE_ATTRIBUTE_READONLY) || (fileAttributesAndFlags & FILE_ATTRIBUTE_READONLY)) && (fileAttributesAndFlags & FILE_FLAG_DELETE_ON_CLOSE))
      return STATUS_CANNOT_DELETE;

    // Truncate should always be used with write access
    if(creationDisposition == TRUNCATE_EXISTING)
      genericDesiredAccess |= GENERIC_WRITE;

    if(g_ImpersonateCallerUser && userTokenHandle != INVALID_HANDLE_VALUE)
    {
      // if g_ImpersonateCallerUser option is on, call the ImpersonateLoggedOnUser function.
      if(!ImpersonateLoggedOnUser(userTokenHandle))
      {
        // handle the error if failed to impersonate
        REX_ERROR(LogDokan, "\tImpersonateLoggedOnUser failed.");
      }
    }

    handle = CreateFileW(filePath,
                         genericDesiredAccess, // GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE,
                         ShareAccess,
                         &securityAttrib, // security attribute
                         creationDisposition,
                         fileAttributesAndFlags, // |FILE_FLAG_NO_BUFFERING,
                         NULL);                  // template file handle

    if(g_ImpersonateCallerUser && userTokenHandle != INVALID_HANDLE_VALUE)
    {
      // Clean Up operation for impersonate
      DWORD lastError = GetLastError();
      CloseHandle(userTokenHandle);
      RevertToSelf();
      SetLastError(lastError);
    }

    if(handle == INVALID_HANDLE_VALUE)
    {
      error = GetLastError();
      REX_ERROR(LogDokan, "\terror code = {}", error);

      status = DokanNtStatusFromWin32(error);
    }
    else
    {
      // Need to update FileAttributes with previous when Overwrite file
      if(fileAttr != INVALID_FILE_ATTRIBUTES && creationDisposition == TRUNCATE_EXISTING)
      {
        SetFileAttributesW(filePath, fileAttributesAndFlags | fileAttr);
      }

      DokanFileInfo->Context = (ULONG64)handle; // save the file handle in Context

      if(creationDisposition == OPEN_ALWAYS || creationDisposition == CREATE_ALWAYS)
      {
        error = GetLastError();
        if(error == ERROR_ALREADY_EXISTS)
        {
          REX_ERROR(LogDokan, "\tOpen an already existing file");
          // Open succeed but we need to inform the driver
          // that the file open and not created by returning STATUS_OBJECT_NAME_COLLISION
          status = STATUS_OBJECT_NAME_COLLISION;
        }
      }
    }
  }

  REX_INFO(LogDokan, "");
  return status;
}

#pragma warning(push)
#pragma warning(disable : 4305)

void DOKAN_CALLBACK MirrorCloseFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  if(DokanFileInfo->Context)
  {
    REX_ERROR(LogDokan, L"CloseFile: {}", filePath);
    REX_ERROR(LogDokan, "\terror : not cleanuped file{}");
    CloseHandle((HANDLE)DokanFileInfo->Context);
    DokanFileInfo->Context = 0;
  }
  else
  {
    REX_INFO(LogDokan, L"Close: {}", filePath);
  }
}

void DOKAN_CALLBACK MirrorCleanup(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  if(DokanFileInfo->Context)
  {
    REX_INFO(LogDokan, L"Cleanup: {}", filePath);
    CloseHandle((HANDLE)(DokanFileInfo->Context));
    DokanFileInfo->Context = 0;
  }
  else
  {
    REX_ERROR(LogDokan, L"Cleanup: {}\tinvalid handle", filePath);
  }

  if(DokanFileInfo->DeleteOnClose)
  {
    // Should already be deleted by CloseHandle
    // if open with FILE_FLAG_DELETE_ON_CLOSE
    REX_INFO(LogDokan, "\tDeleteOnClose");
    if(DokanFileInfo->IsDirectory)
    {
      REX_INFO(LogDokan, "  DeleteDirectory ");
      if(!RemoveDirectoryW(filePath))
      {
        REX_ERROR(LogDokan, "Error code = {}", GetLastError());
      }
      else
      {
        REX_INFO(LogDokan, "success{}");
      }
    }
    else
    {
      REX_INFO(LogDokan, "  DeleteFileW ");
      if(DeleteFileW(filePath) == 0)
      {
        REX_ERROR(LogDokan, " error code = {}", GetLastError());
      }
      else
      {
        REX_INFO(LogDokan, "success{}");
      }
    }
  }
}

NTSTATUS DOKAN_CALLBACK MirrorReadFile(LPCWSTR FileName, LPVOID Buffer, DWORD BufferLength, LPDWORD ReadLength, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle = (HANDLE)DokanFileInfo->Context;
  ULONG offset  = (ULONG)Offset;
  BOOL opened   = FALSE;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"ReadFile : {}", filePath);

  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle, cleanuped?");
    handle = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(handle == INVALID_HANDLE_VALUE)
    {
      DWORD error = GetLastError();
      REX_ERROR(LogDokan, "\tCreateFile error : {}", error);
      return DokanNtStatusFromWin32(error);
    }
    opened = TRUE;
  }

  OVERLAPPED overlap;
  memset(&overlap, 0, sizeof(OVERLAPPED));
  overlap.Offset     = Offset & 0xFFFFFFFF;
  overlap.OffsetHigh = (Offset >> 32) & 0xFFFFFFFF;
  if(!ReadFile(handle, Buffer, BufferLength, ReadLength, &overlap))
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\tread error = %u, buffer length = {}, read length = {}", error, BufferLength, *ReadLength);
    if(opened)
      CloseHandle(handle);
    return DokanNtStatusFromWin32(error);
  }
  else
  {
    REX_INFO(LogDokan, "\tByte to read: {}, Byte read {}, offset {}", BufferLength, *ReadLength, offset);
  }

  if(opened)
    CloseHandle(handle);

  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorWriteFile(LPCWSTR FileName, LPCVOID Buffer, DWORD NumberOfBytesToWrite, LPDWORD NumberOfBytesWritten, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle = (HANDLE)DokanFileInfo->Context;
  BOOL opened   = FALSE;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"WriteFile : {}, offset %I64d, length {}", filePath, Offset, NumberOfBytesToWrite);

  // reopen the file
  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle, cleanuped?");
    handle = CreateFileW(filePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if(handle == INVALID_HANDLE_VALUE)
    {
      DWORD error = GetLastError();
      REX_ERROR(LogDokan, "\tCreateFile error : {}", error);
      return DokanNtStatusFromWin32(error);
    }
    opened = TRUE;
  }

  UINT64 fileSize    = 0;
  DWORD fileSizeLow  = 0;
  DWORD fileSizeHigh = 0;
  fileSizeLow        = GetFileSize(handle, &fileSizeHigh);
  if(fileSizeLow == INVALID_FILE_SIZE)
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\tcan not get a file size error = {}", error);
    if(opened)
      CloseHandle(handle);
    return DokanNtStatusFromWin32(error);
  }

  fileSize = ((UINT64)fileSizeHigh << 32) | fileSizeLow;

  OVERLAPPED overlap;
  memset(&overlap, 0, sizeof(OVERLAPPED));
  if(DokanFileInfo->WriteToEndOfFile)
  {
    overlap.Offset     = 0xFFFFFFFF;
    overlap.OffsetHigh = 0xFFFFFFFF;
  }
  else
  {
    // Paging IO cannot write after allocate file size.
    if(DokanFileInfo->PagingIo)
    {
      if((UINT64)Offset >= fileSize)
      {
        *NumberOfBytesWritten = 0;
        if(opened)
          CloseHandle(handle);
        return STATUS_SUCCESS;
      }

      if(((UINT64)Offset + NumberOfBytesToWrite) > fileSize)
      {
        UINT64 bytes = fileSize - Offset;
        if(bytes >> 32)
        {
          NumberOfBytesToWrite = (DWORD)(bytes & 0xFFFFFFFFUL);
        }
        else
        {
          NumberOfBytesToWrite = (DWORD)bytes;
        }
      }
    }

    if((UINT64)Offset > fileSize)
    {
      // In the mirror sample helperZeroFileData is not necessary. NTFS will
      // zero a hole.
      // But if user's file system is different from NTFS( or other Windows's
      // file systems ) then  users will have to zero the hole themselves.
    }

    overlap.Offset     = Offset & 0xFFFFFFFF;
    overlap.OffsetHigh = (Offset >> 32) & 0xFFFFFFFF;
  }

  if(!WriteFile(handle, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten, &overlap))
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\twrite error = %u, buffer length = {}, write length = {}", error, NumberOfBytesToWrite, *NumberOfBytesWritten);
    if(opened)
      CloseHandle(handle);
    return DokanNtStatusFromWin32(error);
  }
  else
  {
    REX_INFO(LogDokan, L"\twrite {}, offset %I64d{}", *NumberOfBytesWritten, Offset);
  }

  // close the file when it is reopened
  if(opened)
    CloseHandle(handle);

  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorFlushFileBuffers(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle = (HANDLE)DokanFileInfo->Context;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"FlushFileBuffers : {}", filePath);

  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle{}");
    return STATUS_SUCCESS;
  }

  if(FlushFileBuffers(handle))
  {
    return STATUS_SUCCESS;
  }
  else
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\tflush error code = {}", error);
    return DokanNtStatusFromWin32(error);
  }
}

NTSTATUS DOKAN_CALLBACK MirrorGetFileInformation(LPCWSTR FileName, LPBY_HANDLE_FILE_INFORMATION HandleFileInformation, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle = (HANDLE)DokanFileInfo->Context;
  BOOL opened   = FALSE;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"GetFileInfo : {}", filePath);

  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle, cleanuped?");
    handle = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(handle == INVALID_HANDLE_VALUE)
    {
      DWORD error = GetLastError();
      REX_ERROR(LogDokan, "\tCreateFile error : {}", error);
      return DokanNtStatusFromWin32(error);
    }
    opened = TRUE;
  }

  if(!GetFileInformationByHandle(handle, HandleFileInformation))
  {
    REX_ERROR(LogDokan, "\terror code = {}", GetLastError());

    // FileName is a root directory
    // in this case, FindFirstFileW can't get directory information
    if(wcslen(FileName) == 1)
    {
      REX_ERROR(LogDokan, "  root dir");
      HandleFileInformation->dwFileAttributes = GetFileAttributesW(filePath);
    }
    else
    {
      WIN32_FIND_DATAW find;
      ZeroMemory(&find, sizeof(WIN32_FIND_DATAW));
      HANDLE findHandle = FindFirstFileW(filePath, &find);
      if(findHandle == INVALID_HANDLE_VALUE)
      {
        DWORD error = GetLastError();
        REX_ERROR(LogDokan, "\tFindFirstFile error code = {}", error);
        if(opened)
          CloseHandle(handle);
        return DokanNtStatusFromWin32(error);
      }
      HandleFileInformation->dwFileAttributes = find.dwFileAttributes;
      HandleFileInformation->ftCreationTime   = find.ftCreationTime;
      HandleFileInformation->ftLastAccessTime = find.ftLastAccessTime;
      HandleFileInformation->ftLastWriteTime  = find.ftLastWriteTime;
      HandleFileInformation->nFileSizeHigh    = find.nFileSizeHigh;
      HandleFileInformation->nFileSizeLow     = find.nFileSizeLow;
      REX_INFO(LogDokan, "\tFindFiles OK, file size = {}", find.nFileSizeLow);
      FindClose(findHandle);
    }
  }
  else
  {
    REX_INFO(LogDokan, "\tGetFileInformationByHandle success, file size = {}", HandleFileInformation->nFileSizeLow);
  }

  REX_INFO(LogDokan, "FILE ATTRIBUTE  = {}", HandleFileInformation->dwFileAttributes);

  if(opened)
    CloseHandle(handle);

  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorFindFiles(LPCWSTR FileName,
                                               PFillFindData FillFindData, // function pointer
                                               PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  size_t fileLen;
  HANDLE hFind;
  WIN32_FIND_DATAW findData;
  DWORD error;
  int count = 0;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"FindFiles : {}", filePath);

  fileLen = wcslen(filePath);
  if(filePath[fileLen - 1] != L'\\')
  {
    filePath[fileLen++] = L'\\';
  }
  if(fileLen + 1 >= DOKAN_MAX_PATH)
    return STATUS_BUFFER_OVERFLOW;
  filePath[fileLen]     = L'*';
  filePath[fileLen + 1] = L'\0';

  hFind = FindFirstFileW(filePath, &findData);

  if(hFind == INVALID_HANDLE_VALUE)
  {
    error = GetLastError();
    REX_ERROR(LogDokan, "\tinvalid file handle. Error is %u{}", error);
    return DokanNtStatusFromWin32(error);
  }

  // Root folder does not have . and .. folder - we remove them
  BOOLEAN rootFolder = (wcscmp(FileName, L"\\") == 0);
  do
  {
    if(!rootFolder || (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0))
      FillFindData(&findData, DokanFileInfo);
    count++;
  } while(FindNextFileW(hFind, &findData) != 0);

  error = GetLastError();
  FindClose(hFind);

  if(error != ERROR_NO_MORE_FILES)
  {
    REX_ERROR(LogDokan, "\tFindNextFile error. Error is {}", error);
    return DokanNtStatusFromWin32(error);
  }

  REX_INFO(LogDokan, L"\tFindFiles return {} entries in {}", count, filePath);

  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorDeleteFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle = (HANDLE)DokanFileInfo->Context;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);
  REX_INFO(LogDokan, L"DeleteFileW {} - {}", filePath, DokanFileInfo->DeleteOnClose);

  DWORD dwAttrib = GetFileAttributesW(filePath);

  if(dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
    return STATUS_ACCESS_DENIED;

  if(handle && handle != INVALID_HANDLE_VALUE)
  {
    FILE_DISPOSITION_INFO fdi;
    fdi.DeleteFile = DokanFileInfo->DeleteOnClose;
    if(!SetFileInformationByHandle(handle, FileDispositionInfo, &fdi, sizeof(FILE_DISPOSITION_INFO)))
      return DokanNtStatusFromWin32(GetLastError());
  }

  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorDeleteDirectory(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  // HANDLE	handle = (HANDLE)DokanFileInfo->Context;
  HANDLE hFind;
  WIN32_FIND_DATAW findData;
  size_t fileLen;

  ZeroMemory(filePath, sizeof(filePath));
  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"DeleteDirectory {} - {}", filePath, DokanFileInfo->DeleteOnClose);

  if(!DokanFileInfo->DeleteOnClose)
    // Dokan notify that the file is requested not to be deleted.
    return STATUS_SUCCESS;

  fileLen = wcslen(filePath);
  if(filePath[fileLen - 1] != L'\\')
  {
    filePath[fileLen++] = L'\\';
  }
  if(fileLen + 1 >= DOKAN_MAX_PATH)
    return STATUS_BUFFER_OVERFLOW;
  filePath[fileLen]     = L'*';
  filePath[fileLen + 1] = L'\0';

  hFind = FindFirstFileW(filePath, &findData);

  if(hFind == INVALID_HANDLE_VALUE)
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\tDeleteDirectory error code = {}", error);
    return DokanNtStatusFromWin32(error);
  }

  do
  {
    if(wcscmp(findData.cFileName, L"..") != 0 && wcscmp(findData.cFileName, L".") != 0)
    {
      FindClose(hFind);
      REX_ERROR(LogDokan, L"\tDirectory is not empty: {}", findData.cFileName);
      return STATUS_DIRECTORY_NOT_EMPTY;
    }
  } while(FindNextFileW(hFind, &findData) != 0);

  DWORD error = GetLastError();

  FindClose(hFind);

  if(error != ERROR_NO_MORE_FILES)
  {
    REX_ERROR(LogDokan, "\tDeleteDirectory error code = {}", error);
    return DokanNtStatusFromWin32(error);
  }

  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorMoveFile(LPCWSTR FileName, // existing file name
                                              LPCWSTR NewFileName, BOOL ReplaceIfExisting, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  WCHAR newFilePath[DOKAN_MAX_PATH];
  HANDLE handle;
  DWORD bufferSize;
  BOOL result;
  size_t newFilePathLen;

  PFILE_RENAME_INFO renameInfo = NULL;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);
  if(wcslen(NewFileName) && NewFileName[0] != ':')
  {
    GetFilePath(newFilePath, DOKAN_MAX_PATH, NewFileName);
  }
  else
  {
    // For a stream rename, FileRenameInfo expect the FileName param without the filename
    // like :<stream name>:<stream type>
    wcsncpy_s(newFilePath, DOKAN_MAX_PATH, NewFileName, wcslen(NewFileName));
  }

  REX_INFO(LogDokan, L"MoveFile {} -> {}", filePath, newFilePath);
  handle = (HANDLE)DokanFileInfo->Context;
  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle{}");
    return STATUS_INVALID_HANDLE;
  }

  newFilePathLen = wcslen(newFilePath);

  // the PFILE_RENAME_INFO struct has space for one WCHAR for the name at
  // the end, so that
  // accounts for the null terminator

  bufferSize = (DWORD)(sizeof(FILE_RENAME_INFO) + newFilePathLen * sizeof(newFilePath[0]));

  renameInfo = (PFILE_RENAME_INFO)malloc(bufferSize);
  if(!renameInfo)
  {
    return STATUS_BUFFER_OVERFLOW;
  }
  ZeroMemory(renameInfo, bufferSize);

  renameInfo->ReplaceIfExists = ReplaceIfExisting ? TRUE : FALSE;               // some warning about converting BOOL to BOOLEAN
  renameInfo->RootDirectory   = NULL;                                           // hope it is never needed, shouldn't be
  renameInfo->FileNameLength  = (DWORD)newFilePathLen * sizeof(newFilePath[0]); // they want length in bytes

  wcscpy_s(renameInfo->FileName, newFilePathLen + 1, newFilePath);

  result = SetFileInformationByHandle(handle, FileRenameInfo, renameInfo, bufferSize);

  free(renameInfo);

  if(result)
  {
    return STATUS_SUCCESS;
  }
  else
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\tMoveFile error = %u", error);
    return DokanNtStatusFromWin32(error);
  }
}

NTSTATUS DOKAN_CALLBACK MirrorLockFile(LPCWSTR FileName, LONGLONG ByteOffset, LONGLONG Length, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle;
  LARGE_INTEGER offset;
  LARGE_INTEGER length;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"LockFile {}", filePath);

  handle = (HANDLE)DokanFileInfo->Context;
  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle{}");
    return STATUS_INVALID_HANDLE;
  }

  length.QuadPart = Length;
  offset.QuadPart = ByteOffset;

  if(!LockFile(handle, offset.LowPart, offset.HighPart, length.LowPart, length.HighPart))
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\terror code = {}", error);
    return DokanNtStatusFromWin32(error);
  }

  REX_INFO(LogDokan, "\tsuccess{}");
  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorSetEndOfFile(LPCWSTR FileName, LONGLONG ByteOffset, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle;
  LARGE_INTEGER offset;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"SetEndOfFile {}, %I64d", filePath, ByteOffset);

  handle = (HANDLE)DokanFileInfo->Context;
  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle{}");
    return STATUS_INVALID_HANDLE;
  }

  offset.QuadPart = ByteOffset;
  if(!SetFilePointerEx(handle, offset, NULL, FILE_BEGIN))
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\tSetFilePointer error: {}, offset = %I64d{}", error, ByteOffset);
    return DokanNtStatusFromWin32(error);
  }

  if(!SetEndOfFile(handle))
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\tSetEndOfFile error code = {}", error);
    return DokanNtStatusFromWin32(error);
  }

  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorSetAllocationSize(LPCWSTR FileName, LONGLONG AllocSize, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle;
  LARGE_INTEGER fileSize;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"SetAllocationSize {}, %I64d", filePath, AllocSize);

  handle = (HANDLE)DokanFileInfo->Context;
  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle{}");
    return STATUS_INVALID_HANDLE;
  }

  if(GetFileSizeEx(handle, &fileSize))
  {
    if(AllocSize < fileSize.QuadPart)
    {
      fileSize.QuadPart = AllocSize;
      if(!SetFilePointerEx(handle, fileSize, NULL, FILE_BEGIN))
      {
        DWORD error = GetLastError();
        REX_ERROR(LogDokan, "\tSetAllocationSize: SetFilePointer error: {}, offset = {}", error, AllocSize);
        return DokanNtStatusFromWin32(error);
      }
      if(!SetEndOfFile(handle))
      {
        DWORD error = GetLastError();
        REX_ERROR(LogDokan, "\tSetEndOfFile error code = {}", error);
        return DokanNtStatusFromWin32(error);
      }
    }
  }
  else
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\terror code = {}", error);
    return DokanNtStatusFromWin32(error);
  }
  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorSetFileAttributes(LPCWSTR FileName, DWORD FileAttributes, PDOKAN_FILE_INFO DokanFileInfo)
{
  UNREFERENCED_PARAMETER(DokanFileInfo);

  WCHAR filePath[DOKAN_MAX_PATH];

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"SetFileAttributesW {} {:#x}", filePath, FileAttributes);

  if(FileAttributes != 0)
  {
    if(!SetFileAttributesW(filePath, FileAttributes))
    {
      DWORD error = GetLastError();
      REX_ERROR(LogDokan, "\terror code = {}", error);
      return DokanNtStatusFromWin32(error);
    }
  }
  else
  {
    // case FileAttributes == 0 :
    // MS-FSCC 2.6 File Attributes : There is no file attribute with the value 0x00000000
    // because a value of 0x00000000 in the FileAttributes field means that the file attributes for this file MUST NOT be changed when setting basic information for the file
    REX_ERROR(LogDokan, "Set 0 to FileAttributes means MUST NOT be changed. Didn't call SetFileAttributesW function. ");
  }

  REX_INFO(LogDokan, "");
  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorSetFileTime(LPCWSTR FileName, CONST FILETIME* CreationTime, CONST FILETIME* LastAccessTime, CONST FILETIME* LastWriteTime, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"SetFileTime {}", filePath);

  handle = (HANDLE)DokanFileInfo->Context;

  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle{}");
    return STATUS_INVALID_HANDLE;
  }

  if(!SetFileTime(handle, CreationTime, LastAccessTime, LastWriteTime))
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\terror code = {}", error);
    return DokanNtStatusFromWin32(error);
  }

  REX_INFO(LogDokan, "");
  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorUnlockFile(LPCWSTR FileName, LONGLONG ByteOffset, LONGLONG Length, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE handle;
  LARGE_INTEGER length;
  LARGE_INTEGER offset;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"UnlockFile {}", filePath);

  handle = (HANDLE)DokanFileInfo->Context;
  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle{}");
    return STATUS_INVALID_HANDLE;
  }

  length.QuadPart = Length;
  offset.QuadPart = ByteOffset;

  if(!UnlockFile(handle, offset.LowPart, offset.HighPart, length.LowPart, length.HighPart))
  {
    DWORD error = GetLastError();
    REX_ERROR(LogDokan, "\terror code = {}", error);
    return DokanNtStatusFromWin32(error);
  }

  REX_INFO(LogDokan, "\tsuccess{}");
  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorGetFileSecurity(LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG BufferLength, PULONG LengthNeeded, PDOKAN_FILE_INFO DokanFileInfo)
{
  WCHAR filePath[DOKAN_MAX_PATH];
  BOOLEAN requestingSaclInfo;

  UNREFERENCED_PARAMETER(DokanFileInfo);

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"GetFileSecurity {}", filePath);

  MirrorCheckFlag(*SecurityInformation, FILE_SHARE_READ);
  MirrorCheckFlag(*SecurityInformation, OWNER_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, GROUP_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, DACL_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, SACL_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, LABEL_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, ATTRIBUTE_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, SCOPE_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, PROCESS_TRUST_LABEL_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, BACKUP_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, PROTECTED_DACL_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, PROTECTED_SACL_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, UNPROTECTED_DACL_SECURITY_INFORMATION);
  MirrorCheckFlag(*SecurityInformation, UNPROTECTED_SACL_SECURITY_INFORMATION);

  requestingSaclInfo = ((*SecurityInformation & SACL_SECURITY_INFORMATION) || (*SecurityInformation & BACKUP_SECURITY_INFORMATION));

  if(!g_HasSeSecurityPrivilege)
  {
    *SecurityInformation &= ~SACL_SECURITY_INFORMATION;
    *SecurityInformation &= ~BACKUP_SECURITY_INFORMATION;
  }

  REX_INFO(LogDokan, "  Opening new handle with READ_CONTROL access");
  HANDLE handle = CreateFileW(filePath, READ_CONTROL | ((requestingSaclInfo && g_HasSeSecurityPrivilege) ? ACCESS_SYSTEM_SECURITY : 0), FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
                              NULL, // security attribute
                              OPEN_EXISTING,
                              FILE_FLAG_BACKUP_SEMANTICS, // |FILE_FLAG_NO_BUFFERING,
                              NULL);

  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle{}");
    int error = GetLastError();
    return DokanNtStatusFromWin32(error);
  }

  if(!GetUserObjectSecurity(handle, SecurityInformation, SecurityDescriptor, BufferLength, LengthNeeded))
  {
    int error = GetLastError();
    if(error == ERROR_INSUFFICIENT_BUFFER)
    {
      REX_ERROR(LogDokan, "  GetUserObjectSecurity error: ERROR_INSUFFICIENT_BUFFER");
      CloseHandle(handle);
      return STATUS_BUFFER_OVERFLOW;
    }
    else
    {
      REX_ERROR(LogDokan, "  GetUserObjectSecurity error: {}", error);
      CloseHandle(handle);
      return DokanNtStatusFromWin32(error);
    }
  }

  // Ensure the Security Descriptor Length is set
  DWORD securityDescriptorLength = GetSecurityDescriptorLength(SecurityDescriptor);
  REX_INFO(LogDokan, "  GetUserObjectSecurity return true,  *LengthNeeded = securityDescriptorLength ");
  *LengthNeeded = securityDescriptorLength;

  CloseHandle(handle);

  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorSetFileSecurity(LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG SecurityDescriptorLength, PDOKAN_FILE_INFO DokanFileInfo)
{
  HANDLE handle;
  WCHAR filePath[DOKAN_MAX_PATH];

  UNREFERENCED_PARAMETER(SecurityDescriptorLength);

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"SetFileSecurity {}", filePath);

  handle = (HANDLE)DokanFileInfo->Context;
  if(!handle || handle == INVALID_HANDLE_VALUE)
  {
    REX_ERROR(LogDokan, "\tinvalid handle{}");
    return STATUS_INVALID_HANDLE;
  }

  if(!SetUserObjectSecurity(handle, SecurityInformation, SecurityDescriptor))
  {
    int error = GetLastError();
    REX_ERROR(LogDokan, "  SetUserObjectSecurity error: {}", error);
    return DokanNtStatusFromWin32(error);
  }
  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorGetVolumeInformation(LPWSTR VolumeNameBuffer, DWORD VolumeNameSize, LPDWORD VolumeSerialNumber, LPDWORD MaximumComponentLength, LPDWORD FileSystemFlags, LPWSTR FileSystemNameBuffer, DWORD FileSystemNameSize,
                                                          PDOKAN_FILE_INFO DokanFileInfo)
{
  UNREFERENCED_PARAMETER(DokanFileInfo);

  WCHAR volumeRoot[4];
  DWORD fsFlags = 0;

  rsl::string file_system_name_buffer = rsl::to_string(rsl::wstring_view(FileSystemNameBuffer));

  wcscpy_s(VolumeNameBuffer, VolumeNameSize, g_volume_name.c_str());

  if(VolumeSerialNumber)
    *VolumeSerialNumber = 0x19831116;
  if(MaximumComponentLength)
    *MaximumComponentLength = 255;
  if(FileSystemFlags)
  {
    *FileSystemFlags = FILE_SUPPORTS_REMOTE_STORAGE | FILE_UNICODE_ON_DISK | FILE_PERSISTENT_ACLS | FILE_NAMED_STREAMS;
    if(g_CaseSensitive)
      *FileSystemFlags = FILE_CASE_SENSITIVE_SEARCH | FILE_CASE_PRESERVED_NAMES;
  }

  volumeRoot[0] = g_blob_store_path[0];
  volumeRoot[1] = ':';
  volumeRoot[2] = '\\';
  volumeRoot[3] = '\0';

  if(GetVolumeInformationW(volumeRoot, NULL, 0, NULL, MaximumComponentLength, &fsFlags, FileSystemNameBuffer, FileSystemNameSize))
  {
    if(FileSystemFlags)
      *FileSystemFlags &= fsFlags;

    if(MaximumComponentLength)
    {
      REX_INFO(LogDokan, "GetVolumeInformation: max component length {}", *MaximumComponentLength);
    }
    if(FileSystemNameBuffer)
    {
      REX_INFO(LogDokan, "GetVolumeInformation: file system name {}", file_system_name_buffer);
    }
    if(FileSystemFlags)
    {
      REX_INFO(LogDokan, "GetVolumeInformation: got file system flags {}, returning {:#x}", fsFlags, *FileSystemFlags);
    }
  }
  else
  {
    REX_ERROR(LogDokan, "GetVolumeInformation: unable to query underlying fs, using defaults.  Last error = {}", GetLastError());

    // File system name could be anything up to 10 characters.
    // But Windows check few feature availability based on file system name.
    // For this, it is recommended to set NTFS or FAT here.
    wcscpy_s(FileSystemNameBuffer, FileSystemNameSize, L"NTFS");
  }

  return STATUS_SUCCESS;
}

// Uncomment the function and set dokan_operations.GetDiskFreeSpace to personalize disk space
/*
NTSTATUS DOKAN_CALLBACK MirrorDokanGetDiskFreeSpace(
    PULONGLONG FreeBytesAvailable, PULONGLONG TotalNumberOfBytes,
    PULONGLONG TotalNumberOfFreeBytes, PDOKAN_FILE_INFO DokanFileInfo) {
  UNREFERENCED_PARAMETER(DokanFileInfo);

  *FreeBytesAvailable = (ULONGLONG)(512 * 1024 * 1024);
  *TotalNumberOfBytes = 9223372036854775807;
  *TotalNumberOfFreeBytes = 9223372036854775807;

  return STATUS_SUCCESS;
}
*/

NTSTATUS DOKAN_CALLBACK MirrorDokanGetDiskFreeSpace(PULONGLONG FreeBytesAvailable, PULONGLONG TotalNumberOfBytes, PULONGLONG TotalNumberOfFreeBytes, PDOKAN_FILE_INFO DokanFileInfo)
{
  UNREFERENCED_PARAMETER(DokanFileInfo);
  WCHAR DriveLetter[3] = {'C', ':', 0};
  PWCHAR RootPathName;

  if(g_blob_store_path[0] == L'\\')
  { // UNC as Root
    RootPathName = g_blob_store_path.data();
  }
  else
  {
    DriveLetter[0] = g_blob_store_path[0];
    RootPathName   = DriveLetter;
  }

  if(!GetDiskFreeSpaceExW(RootPathName, (PULARGE_INTEGER)FreeBytesAvailable, (PULARGE_INTEGER)TotalNumberOfBytes, (PULARGE_INTEGER)TotalNumberOfFreeBytes))
  {
    DWORD error                = GetLastError();
    rsl::string root_path_name = rsl::to_string(rsl::wstring_view(RootPathName));
    REX_ERROR(LogDokan, "GetDiskFreeSpaceEx failed for path {}", root_path_name);
    return DokanNtStatusFromWin32(error);
  }
  return STATUS_SUCCESS;
}

/**
 * Avoid #include <winternl.h> which as conflict with FILE_INFORMATION_CLASS
 * definition.
 * This only for MirrorFindStreams. Link with ntdll.lib still required.
 *
 * Not needed if you're not using NtQueryInformationFile!
 *
 * BEGIN
 */
#pragma warning(push)
#pragma warning(disable : 4201)
typedef struct _IO_STATUS_BLOCK
{
  union
  {
    NTSTATUS Status;
    PVOID Pointer;
  } DUMMYUNIONNAME;

  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
#pragma warning(pop)

NTSYSCALLAPI NTSTATUS NTAPI NtQueryInformationFile(_In_ HANDLE FileHandle, _Out_ PIO_STATUS_BLOCK IoStatusBlock, _Out_writes_bytes_(Length) PVOID FileInformation, _In_ ULONG Length, _In_ FILE_INFORMATION_CLASS FileInformationClass);
/**
 * END
 */

NTSTATUS DOKAN_CALLBACK MirrorFindStreams(LPCWSTR FileName, PFillFindStreamData FillFindStreamData, PVOID FindStreamContext, PDOKAN_FILE_INFO DokanFileInfo)
{
  UNREFERENCED_PARAMETER(DokanFileInfo);

  WCHAR filePath[DOKAN_MAX_PATH];
  HANDLE hFind;
  WIN32_FIND_STREAM_DATA findData;
  DWORD error;
  int count = 0;

  GetFilePath(filePath, DOKAN_MAX_PATH, FileName);

  REX_INFO(LogDokan, L"FindStreams :{}", filePath);

  hFind = FindFirstStreamW(filePath, FindStreamInfoStandard, &findData, 0);

  if(hFind == INVALID_HANDLE_VALUE)
  {
    error = GetLastError();
    REX_ERROR(LogDokan, "\tinvalid file handle. Error is %u{}", error);
    return DokanNtStatusFromWin32(error);
  }

  BOOL bufferFull = FillFindStreamData(&findData, FindStreamContext);
  if(bufferFull)
  {
    count++;
    while(FindNextStreamW(hFind, &findData) != 0)
    {
      bufferFull = FillFindStreamData(&findData, FindStreamContext);
      if(!bufferFull)
        break;
      count++;
    }
  }

  error = GetLastError();
  FindClose(hFind);

  if(!bufferFull)
  {
    REX_ERROR(LogDokan, L"\tFindStreams returned {} entries in {} with STATUS_BUFFER_OVERFLOW", count, filePath);
    // https://msdn.microsoft.com/en-us/library/windows/hardware/ff540364(v=vs.85).aspx
    return STATUS_BUFFER_OVERFLOW;
  }

  if(error != ERROR_HANDLE_EOF)
  {
    REX_ERROR(LogDokan, "\tFindNextStreamW error. Error is %u{}", error);
    return DokanNtStatusFromWin32(error);
  }

  REX_INFO(LogDokan, L"\tFindStreams return {} entries in {}", count, filePath);

  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorMounted(LPCWSTR MountPoint, PDOKAN_FILE_INFO DokanFileInfo)
{
  UNREFERENCED_PARAMETER(DokanFileInfo);

  rsl::string mount_point = rsl::to_string(rsl::wstring_view(MountPoint));
  REX_INFO(LogDokan, "Mounted as {}", mount_point);
  return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MirrorUnmounted(PDOKAN_FILE_INFO DokanFileInfo)
{
  UNREFERENCED_PARAMETER(DokanFileInfo);

  REX_INFO(LogDokan, "Unmounted");
  return STATUS_SUCCESS;
}

#pragma warning(pop)

BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
  switch(dwCtrlType)
  {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
      SetConsoleCtrlHandler(CtrlHandler, FALSE);
      DokanRemoveMountPoint(g_mount_point.c_str());
      return TRUE;
    default: return FALSE;
  }
}

namespace dokan_fs
{
  bool setup_ctrl_handler()
  {
    return SetConsoleCtrlHandler(CtrlHandler, TRUE);
  }

  DOKAN_OPTIONS parse_cmdline()
  {
    g_DebugMode = FALSE;
    g_UseStdErr = FALSE;
    g_CaseSensitive = FALSE;

    DOKAN_OPTIONS dokan_options;
    ZeroMemory(&dokan_options, sizeof(DOKAN_OPTIONS));
    dokan_options.Version = DOKAN_VERSION;
    dokan_options.Options |= DOKAN_OPTION_ALT_STREAM;

    g_blob_store_path = rsl::to_wstring(rex::cmdline::get_argument("BlobStorePath").value_or(""));
    g_mount_point = rsl::to_wstring(rex::cmdline::get_argument("MountPath").value_or(""));
    dokan_options.MountPoint = g_mount_point;

    if (rex::cmdline::get_argument("SingleThreaded"))
    {
      dokan_options.SingleThread = TRUE;
    }
    if (rex::cmdline::get_argument("DebugApp"))
    {
      g_DebugMode = TRUE;
      dokan_options.Options |= DOKAN_OPTION_DEBUG;
    }
    if (rex::cmdline::get_argument("EnableStdErr"))
    {
      g_UseStdErr = TRUE;
      dokan_options.Options |= DOKAN_OPTION_STDERR;
    }
    if (rex::cmdline::get_argument("UseRemovableDrive"))
    {
      dokan_options.Options |= DOKAN_OPTION_REMOVABLE;
    }
    if (rex::cmdline::get_argument("ReadOnly"))
    {
      dokan_options.Options |= DOKAN_OPTION_WRITE_PROTECT;
    }
    if (rex::cmdline::get_argument("EnableCaseSensitive"))
    {
      dokan_options.Options |= DOKAN_OPTION_CASE_SENSITIVE;
      g_CaseSensitive = TRUE;
    }
    if (rex::cmdline::get_argument("UseMountManager"))
    {
      dokan_options.Options |= DOKAN_OPTION_MOUNT_MANAGER;
    }
    if (rex::cmdline::get_argument("EnableForCurrentSessionOnly"))
    {
      dokan_options.Options |= DOKAN_OPTION_CURRENT_SESSION;
    }
    if (rex::cmdline::get_argument("ShowAsNetworkDevice"))
    {
      g_unc_name = rsl::to_wstring(rex::cmdline::get_argument("ShowAsNetworkDevice").value());
      dokan_options.UNCName = g_unc_name;
      REX_INFO(LogDokan, "UNC Name: {}", rex::cmdline::get_argument("ShowAsNetworkDevice").value());
    }
    if (rex::cmdline::get_argument("EnableCallerUser"))
    {
      g_ImpersonateCallerUser = TRUE;
    }
    if (rex::cmdline::get_argument("AllocationUnitSize"))
    {
      dokan_options.AllocationUnitSize = rsl::stoi(rex::cmdline::get_argument("AllocationUnitSize").value()).value();
    }
    if (rex::cmdline::get_argument("SectorSize"))
    {
      dokan_options.SectorSize = rsl::stoi(rex::cmdline::get_argument("SectorSize").value()).value();
    }
    if (rex::cmdline::get_argument("EnableLockMode"))
    {
      dokan_options.Options |= DOKAN_OPTION_FILELOCK_USER_MODE;
    }
    if (rex::cmdline::get_argument("Timeout"))
    {
      dokan_options.Timeout = rsl::stoi(rex::cmdline::get_argument("Timeout").value()).value();
    }
    if (rex::cmdline::get_argument("HelpAntiVirus"))
    {
      REX_WARN(LogDokan, "FCB GCt enabling is currently not supported");
    }
    if (rex::cmdline::get_argument("AllowUnmount"))
    {
      dokan_options.Options |= DOKAN_OPTION_ENABLE_UNMOUNT_NETWORK_DRIVE;
    }
    if (rex::cmdline::get_argument("EnableDriveLogs"))
    {
      dokan_options.Options |= DOKAN_OPTION_DISPATCH_DRIVER_LOGS;
    }
    if (rex::cmdline::get_argument("VolumeName"))
    {
      g_volume_name = rsl::to_wstring(rex::cmdline::get_argument("VolumeName").value());
    }

    return dokan_options;
  }

  int startup_error_handling(const DOKAN_OPTIONS& dokanOptions)
  {
    if (g_unc_name.length() && !(dokanOptions.Options & DOKAN_OPTION_NETWORK))
    {
      REX_ERROR(LogDokan, "  Warning: UNC provider name should be set on network drive only.");
      return EXIT_FAILURE;
    }

    if (dokanOptions.Options & DOKAN_OPTION_NETWORK && dokanOptions.Options & DOKAN_OPTION_MOUNT_MANAGER)
    {
      REX_ERROR(LogDokan, "Mount manager cannot be used on network drive.");
      return EXIT_FAILURE;
    }

    if (!(dokanOptions.Options & DOKAN_OPTION_MOUNT_MANAGER) && g_mount_point.empty())
    {
      REX_ERROR(LogDokan, "Mount Point required.");
      return EXIT_FAILURE;
    }

    if ((dokanOptions.Options & DOKAN_OPTION_MOUNT_MANAGER) && (dokanOptions.Options & DOKAN_OPTION_CURRENT_SESSION))
    {
      REX_ERROR(LogDokan, "Mount Manager always mount the drive for all user sessions.");
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }

  int display_dokan_error(int status)
  {
    switch (status)
    {
    case DOKAN_SUCCESS: REX_INFO(LogDokan, "Success"); break;
    case DOKAN_ERROR: REX_ERROR(LogDokan, "Error"); break;
    case DOKAN_DRIVE_LETTER_ERROR: REX_ERROR(LogDokan, "Bad Drive letter"); break;
    case DOKAN_DRIVER_INSTALL_ERROR: REX_ERROR(LogDokan, "Can't install driver"); break;
    case DOKAN_START_ERROR:
      REX_ERROR(LogDokan, "Failed to start Driver.");
      REX_ERROR(LogDokan, "This can be because one of the following reasons");
      REX_ERROR(LogDokan, "- Invalid driver version");
      REX_ERROR(LogDokan, "- MountPoint / UNCName proved are not null terminated");
      REX_ERROR(LogDokan, "- Network filesystem is enabled without UNCName");
      REX_ERROR(LogDokan, "- Existing mount found without mount manager enabled");
      REX_ERROR(LogDokan, "- Invalid volume security descriptor length");
      REX_ERROR(LogDokan, "- Mount point was still not assigned after forcing");
      REX_ERROR(LogDokan, "- Failed to set reparse point on mount point");
      break;
    case DOKAN_MOUNT_ERROR: REX_ERROR(LogDokan, "Can't assign a drive letter"); break;
    case DOKAN_MOUNT_POINT_ERROR: REX_ERROR(LogDokan, "Mount point error"); break;
    case DOKAN_VERSION_ERROR: REX_ERROR(LogDokan, "Version error"); break;
    default: REX_ERROR(LogDokan, "Unknown error: {}", status); break;
    }

    return 0;
  }

  DOKAN_OPERATIONS init_dokan_operations()
  {
    DOKAN_OPERATIONS dokan_operations;
    ZeroMemory(&dokan_operations, sizeof(DOKAN_OPERATIONS));
    dokan_operations.ZwCreateFile = MirrorCreateFile;
    dokan_operations.Cleanup = MirrorCleanup;
    dokan_operations.CloseFile = MirrorCloseFile;
    dokan_operations.ReadFile = MirrorReadFile;
    dokan_operations.WriteFile = MirrorWriteFile;
    dokan_operations.FlushFileBuffers = MirrorFlushFileBuffers;
    dokan_operations.GetFileInformation = MirrorGetFileInformation;
    dokan_operations.FindFiles = MirrorFindFiles;
    dokan_operations.FindFilesWithPattern = NULL;
    dokan_operations.SetFileAttributes = MirrorSetFileAttributes;
    dokan_operations.SetFileTime = MirrorSetFileTime;
    dokan_operations.DeleteFile = MirrorDeleteFile;
    dokan_operations.DeleteDirectory = MirrorDeleteDirectory;
    dokan_operations.MoveFile = MirrorMoveFile;
    dokan_operations.SetEndOfFile = MirrorSetEndOfFile;
    dokan_operations.SetAllocationSize = MirrorSetAllocationSize;
    dokan_operations.LockFile = MirrorLockFile;
    dokan_operations.UnlockFile = MirrorUnlockFile;
    dokan_operations.GetFileSecurity = MirrorGetFileSecurity;
    dokan_operations.SetFileSecurity = MirrorSetFileSecurity;
    dokan_operations.GetDiskFreeSpace = MirrorDokanGetDiskFreeSpace;
    dokan_operations.GetVolumeInformation = MirrorGetVolumeInformation;
    dokan_operations.Unmounted = MirrorUnmounted;
    dokan_operations.FindStreams = MirrorFindStreams;
    dokan_operations.Mounted = MirrorMounted;

    return dokan_operations;
  }

  bool initialize()
  {
    if (!setup_ctrl_handler())
    {
      REX_ERROR(LogDokan, "Failed to set up ctrl handler");
      return false;
    }

    DOKAN_OPTIONS dokan_options = parse_cmdline();

    if (startup_error_handling(dokan_options))
    {
      REX_ERROR(LogDokan, "Invalid dokan options");
      return false;
    }

    // Add security name privilege. Required here to handle GetFileSecurity properly.
    g_HasSeSecurityPrivilege = AddSeSecurityNamePrivilege();
    if(!g_HasSeSecurityPrivilege)
    {
      REX_ERROR(LogDokan, "[Mirror] Failed to add security privilege to process"
                          "\t=> GetFileSecurity/SetFileSecurity may not work properly"
                          "\t=> Please restart mirror sample with administrator rights to fix it");
    }

    if(g_ImpersonateCallerUser && !g_HasSeSecurityPrivilege)
    {
      REX_ERROR(LogDokan, "[Mirror] Impersonate Caller User requires administrator right to work properly"
                          "\t=> Other users may not use the drive properly"
                          "\t=> Please restart mirror sample with administrator rights to fix it");
    }

    DOKAN_OPERATIONS dokan_operations = init_dokan_operations();

    DokanInit();
    int status = DokanMain(&dokan_options, &dokan_operations);
    DokanShutdown();
    
    display_dokan_error(status);

    return EXIT_SUCCESS;
  }
  void update() {}
  void shutdown() {}
} // namespace dokan_fs

namespace rex
{
  ApplicationCreationParams app_entry(PlatformCreationParams& platformParams)
  {
    ApplicationCreationParams app_params(platformParams);

    app_params.engine_params.app_init_func     = dokan_fs::initialize;
    app_params.engine_params.app_update_func   = dokan_fs::update;
    app_params.engine_params.app_shutdown_func = dokan_fs::shutdown;

    app_params.create_window = false;

    return app_params;
  }
} // namespace rex