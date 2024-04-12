#include "rex_windows/filesystem/drive_handle.h"

#include "rex_engine/diagnostics/win/win_call.h"

#include <Windows.h>

namespace rex
{
  namespace win32
  {
    DriveHandle::DriveHandle(char8 driveLetter)
        : m_drive_handle(INVALID_HANDLE_VALUE)
        , m_drive_letter(driveLetter)
    {
      VolumePath drive_path = volume_path();

      auto handle = WIN_CALL_IGNORE(CreateFileA(drive_path.c_str(),
                                                FILE_TRAVERSE, // set to traverse so we don't need to run in admin mode
                                                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL),
                                    ERROR_INVALID_HANDLE);

      m_drive_handle = rsl::win::handle(handle);
    }

    VolumePath DriveHandle::volume_path() const
    {
      return rsl::format("\\\\.\\{}:", m_drive_letter);
    }

    bool DriveHandle::is_valid() const
    {
      return m_drive_handle.is_valid();
    }

    const rsl::win::handle& DriveHandle::handle() const
    {
      return m_drive_handle;
    }
  } // namespace win32
} // namespace rex