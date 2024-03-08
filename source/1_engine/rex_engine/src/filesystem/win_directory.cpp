#include "rex_engine/platform/win/filesystem/win_directory.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/time/win/win_time_functions.h"
#include "rex_std/bonus/platform.h"

namespace rex
{
  namespace directory
  {
    namespace internal
    {
      rsl::win::handle open_file_for_attribs(rsl::string_view path)
      {
        rsl::win::handle file(CreateFileA(path.data(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr));

        return file;
      }
    }

    // Create a new directory
    void create(rsl::string_view path)
    {
      rsl::string full_path = vfs::create_full_path(path);
      if (exists(full_path))
      {
        return;
      }

      WIN_CALL(CreateDirectoryA(full_path.c_str(), NULL));
    }
    // Delete a directory
    void del(rsl::string_view path)
    {
      rsl::string full_path = vfs::create_full_path(path);
      RemoveDirectoryA(full_path.c_str());
    }
    // Return if a directory exists
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
        return true;
      }

      return false;
    }
    // Copy a directory and its content
    void copy(rsl::string_view src, rsl::string_view dst)
    {
      rsl::string full_src = vfs::create_full_path(src);
      rsl::string full_dst = vfs::create_full_path(dst);

      rsl::vector<rsl::string> all_files = list_files(full_src);
      rsl::vector<rsl::string> all_dirs = list_dirs(full_src);

      create(full_dst);

      for (rsl::string_view file_entry : all_files)
      {
        rsl::string rel_path = path::rel_path(file_entry, full_src);
        rsl::string entry_dst = path::join(full_dst, rel_path);
        file::copy(file_entry, entry_dst);
      }

      for (rsl::string_view dir_entry : all_dirs)
      {
        rsl::string rel_path = path::rel_path(dir_entry, full_src);
        rsl::string entry_dst = path::join(full_dst, rel_path);
        create(entry_dst);
        copy(dir_entry, entry_dst);
      }
    }
    // Move/Rename a directory
    void move(rsl::string_view src, rsl::string_view dst)
    {
      rsl::string full_src = vfs::create_full_path(src);
      rsl::string full_dst = vfs::create_full_path(dst);

      MoveFileA(full_src.c_str(), full_dst.c_str());
    }
    // List all entries under a directory
    rsl::vector<rsl::string> list_entries(rsl::string_view path)
    {
      WIN32_FIND_DATAA ffd;
      rsl::big_stack_string dir_search(path);
      dir_search += "\\*";
      HANDLE hFind = FindFirstFileA(path.data(), &ffd);

      if (hFind == INVALID_HANDLE_VALUE)
      {
        FindClose(hFind);
        return {};
      }

      rsl::vector<rsl::string> result;
      do
      {
        rsl::string_view name = ffd.cFileName;
        result.push_back(path::join(path, name));
      } while (FindNextFile(hFind, &ffd) != 0);

      return result;
    }
    // List all directories under a directory
    rsl::vector<rsl::string> list_dirs(rsl::string_view path)
    {
      WIN32_FIND_DATAA ffd;
      rsl::big_stack_string dir_search(path);
      dir_search += "\\*";
      HANDLE hFind = FindFirstFileA(path.data(), &ffd);

      if (hFind == INVALID_HANDLE_VALUE)
      {
        FindClose(hFind);
        return {};
      }

      rsl::vector<rsl::string> result;
      do
      {
        rsl::string_view name = ffd.cFileName;
        rsl::string full_filename = path::join(path, name);
        if (exists(full_filename))
        {
          result.push_back(rsl::move(full_filename));
        }
      } while (FindNextFile(hFind, &ffd) != 0);

      return result;
    }
    // List all files under a directory
    rsl::vector<rsl::string> list_files(rsl::string_view path)
    {
      WIN32_FIND_DATAA ffd{};
      rsl::big_stack_string dir_search(path);
      dir_search += "\\*";
      HANDLE hFind = FindFirstFileA(dir_search.data(), &ffd);

      if (hFind == INVALID_HANDLE_VALUE)
      {
        FindClose(hFind);
        return {};
      }

      rsl::vector<rsl::string> result;
      do
      {
        s32 length = rsl::strlen(ffd.cFileName);
        rsl::string_view name(ffd.cFileName, length);
        rsl::string full_filename = path::join(path, name);
        if (file::exists(full_filename))
        {
          result.push_back(rsl::move(full_filename));
        }
      } while (FindNextFileA(hFind, &ffd) != 0);

      // FindNextfile sets the error to ERROR_NO_MORE_FILES
      // if there are no more files found
      // We reset it here to avoid any confusion
      SetLastError(ERROR_SUCCESS);

      return result;
    }

    // Return the creation time of a directory
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
    // Return the access time of a directory
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
    // Return the modification time of a directory
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