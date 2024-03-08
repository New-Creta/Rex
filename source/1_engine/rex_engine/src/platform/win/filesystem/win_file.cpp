#include "rex_engine/platform/win/filesystem/win_file.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/engine/numeric.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/time/win/win_time_functions.h"
#include <Windows.h>

namespace rex
{
  namespace file
  {
    namespace internal
    {
      void append_to_file(HANDLE handle)
      {
        SetFilePointer(handle, 0, NULL, FILE_END);
        SetEndOfFile(handle);
      }

      void trunc_file(HANDLE handle)
      {
        SetFilePointer(handle, 0, NULL, FILE_BEGIN);
        SetEndOfFile(handle);
      }

      rsl::win::handle open_file_for_attribs(rsl::string_view path)
      {
        rsl::win::handle file(CreateFileA(path.data(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr));

        return file;
      }
    }

    // Append lines to a file
    void append_lines(rsl::string_view path, const rsl::vector<rsl::string>& lines)
    {
      if (is_readonly(path))
      {
        REX_ERROR(FileLog, "File \"{}\" is read only. Cannot append lines");
        return;
      }

      rsl::string full_path = rex::vfs::create_full_path(path);

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(full_path.c_str(),               // Path to file
        GENERIC_READ,              // General read and write access
        FILE_SHARE_READ,           // Other processes can also read the file
        NULL,                      // No SECURITY_ATTRIBUTES
        OPEN_EXISTING,             // Open the file, only if it exists
        FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
        NULL                       // No template file
      ),
        ERROR_ALREADY_EXISTS));

      internal::append_to_file(handle.get());

      for (rsl::string_view line : lines)
      {
        WriteFile(handle.get(), line.data(), line.length(), NULL, NULL);
      }
    }
    // Append text to a file
    void append_text(rsl::string_view path, rsl::string_view txt)
    {
      if (is_readonly(path))
      {
        REX_ERROR(FileLog, "File \"{}\" is read only. Cannot append lines");
        return;
      }

      rsl::string full_path = rex::vfs::create_full_path(path);

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(full_path.c_str(),               // Path to file
        GENERIC_READ,              // General read and write access
        FILE_SHARE_READ,           // Other processes can also read the file
        NULL,                      // No SECURITY_ATTRIBUTES
        OPEN_EXISTING,             // Open the file, only if it exists
        FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
        NULL                       // No template file
      ),
        ERROR_ALREADY_EXISTS));

      internal::append_to_file(handle.get());

      WriteFile(handle.get(), txt.data(), txt.length(), NULL, NULL);
    }
    // Trunc a file, removing all content
    void trunc(rsl::string_view path)
    {
      if (is_readonly(path))
      {
        REX_ERROR(FileLog, "File \"{}\" is read only. Cannot append lines");
        return;
      }

      rsl::string full_path = rex::vfs::create_full_path(path);

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(full_path.c_str(),               // Path to file
        GENERIC_READ,              // General read and write access
        FILE_SHARE_READ,           // Other processes can also read the file
        NULL,                      // No SECURITY_ATTRIBUTES
        OPEN_EXISTING,             // Open the file, only if it exists
        FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
        NULL                       // No template file
      ),
        ERROR_ALREADY_EXISTS));

      internal::trunc_file(handle.get());
    }
    // Copy a file, overwiting an existing one is possible
    void copy(rsl::string_view src, rsl::string_view dst, OverwriteIfExist overwriteIfExist)
    {
      rsl::string full_src = vfs::create_full_path(src);
      rsl::string full_dst = vfs::create_full_path(dst);
      CopyFileA(full_src.c_str(), full_dst.c_str(), !overwriteIfExist);
    }
    // Move/Rename a file, overwriting an existing one is possible
    void move(rsl::string_view src, rsl::string_view dst)
    {
      rsl::string full_src = vfs::create_full_path(src);
      rsl::string full_dst = vfs::create_full_path(dst);

      if (file::exists(full_dst))
      {
        REX_ERROR(FileLog, "Cannot move to \"{}\", file already exists", dst);
      }

      MoveFileA(full_src.c_str(), full_dst.c_str());
    }
    // Create a new empty file
    void create(rsl::string_view path)
    {
      rsl::string full_path = vfs::create_full_path(path);

      if (file::exists(full_path))
      {
        return;
      }

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(full_path.c_str(),               // Path to file
        GENERIC_READ,              // General read and write access
        FILE_SHARE_READ,           // Other processes can also read the file
        NULL,                      // No SECURITY_ATTRIBUTES
        CREATE_NEW,                // Open the file, only if it exists
        FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
        NULL                       // No template file
      ),
        ERROR_ALREADY_EXISTS));
    }
    // Delete a file
    void del(rsl::string_view path)
    {
      rsl::string full_path = vfs::create_full_path(path);

      if (!file::exists(full_path))
      {
        return;
      }

      DeleteFileA(full_path.c_str());
    }
    // return the file size
    card64 size(rsl::string_view path)
    {
      rsl::string full_path = vfs::create_full_path(path);
      const rsl::win::handle file = internal::open_file_for_attribs(full_path);

      // When we have an invalid handle, we simply return the input
      if (!file.is_valid())
      {
        return -1;
      }

      DWORD high_word = 0;
      DWORD const low_word = GetFileSize(file.get(), &high_word);
      return rex::merge_int32_to_int64(high_word, low_word);
    }
    // Check if a file exists
    bool exists(rsl::string_view path)
    {
      rsl::string full_path = vfs::create_full_path(path);
      const DWORD attribs = GetFileAttributesA(full_path.c_str());

      if (attribs == INVALID_FILE_ATTRIBUTES)
      {
        return false;
      }

      if ((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0u)
      {
        return false;
      }

      return true;

    }
    // Check if a file is marked read only
    bool is_readonly(rsl::string_view path)
    {
      rsl::string full_path = vfs::create_full_path(path);
      if (!exists(full_path))
      {
        return false;
      }

      const DWORD attribs = GetFileAttributesA(full_path.c_str());
      return attribs & FILE_ATTRIBUTE_READONLY;
    }
    // Set a file to be readonly
    void set_readonly(rsl::string_view path)
    {
      rsl::string full_path = vfs::create_full_path(path);
      if (!exists(full_path))
      {
        return;
      }

      DWORD attribs = GetFileAttributesA(full_path.c_str());
      attribs |= FILE_ATTRIBUTE_READONLY;
      SetFileAttributesA(full_path.c_str(), attribs);
    }
    // Remove the readonly flag of a file
    void remove_readonly(rsl::string_view path)
    {
      rsl::string full_path = vfs::create_full_path(path);
      if (!exists(full_path))
      {
        return;
      }

      DWORD attribs = GetFileAttributesA(full_path.c_str());
      attribs &= ~FILE_ATTRIBUTE_READONLY;
      SetFileAttributesA(full_path.c_str(), attribs);
    }
    // Return the creation time of a file
    rsl::time_point creation_time(rsl::string_view path)
    {
      const rsl::win::handle file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if (!file.is_valid())
      {
        return rsl::time_point();
      }

      FILETIME creation_time{};
      GetFileTime(file.get(), &creation_time, nullptr, nullptr);
      SYSTEMTIME sys_time = rsl::win::to_local_sys_time(creation_time);
      return rsl::timepoint_from_systime(sys_time);

    }
    // Return the access time of a file
    rsl::time_point access_time(rsl::string_view path)
    {
      rsl::win::handle const file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if (!file.is_valid())
      {
        return rsl::time_point();
      }

      FILETIME access_time{};
      GetFileTime(file.get(), nullptr, &access_time, nullptr);
      SYSTEMTIME sys_time = rsl::win::to_local_sys_time(access_time);
      return rsl::timepoint_from_systime(sys_time);
    }
    // Return the modification time of a file
    rsl::time_point modification_time(rsl::string_view path)
    {
      rsl::win::handle const file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if (!file.is_valid())
      {
        return rsl::time_point();
      }

      FILETIME modification_time{};
      GetFileTime(file.get(), nullptr, nullptr, &modification_time);
      SYSTEMTIME sys_time = rsl::win::to_local_sys_time(modification_time);
      return rsl::timepoint_from_systime(sys_time);
    }
  }
}