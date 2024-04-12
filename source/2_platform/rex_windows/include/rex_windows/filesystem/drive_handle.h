#pragma once

#include "rex_engine/types.h"
#include "rex_std/bonus/platform.h"

namespace rex
{
  namespace win32
  {
    using VolumePath = rsl::stack_string<char8, 8>;

    class DriveHandle
    {
    public:
      DriveHandle(char8 driveLetter);

      VolumePath volume_path() const;
      bool is_valid() const;
      const rsl::win::handle& handle() const;

    private:
      rsl::win::handle m_drive_handle;
      char8 m_drive_letter;
    };
  } // namespace win32
} // namespace rex