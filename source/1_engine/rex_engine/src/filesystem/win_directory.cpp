#include "rex_engine/platform/win/filesystem/win_directory.h"

#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/time/win/win_time_functions.h"

namespace rex
{
  namespace directory
  {
    namespace internal
    {
      rsl::win::handle open_file_for_attribs(rsl::string_view path)
      {
        return rsl::win::handle(CreateFileA(path.data(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr));
      }
    } // namespace internal

    // Create a new directory
    void create(rsl::string_view path)
    {
      const rsl::string full_path = vfs::create_full_path(path);
      if(exists(full_path))
      {
        return;
      }

      WIN_CALL(CreateDirectoryA(full_path.c_str(), nullptr));
    }
    // Delete a directory
    void del(rsl::string_view path)
    {
      const rsl::string full_path = vfs::create_full_path(path);
      RemoveDirectoryA(full_path.c_str());
    }
    // Return if a directory exists
    bool exists(rsl::string_view path)
    {
      const rsl::string full_path = vfs::create_full_path(path);
      const DWORD attribs   = GetFileAttributesA(full_path.c_str());

      if(attribs == INVALID_FILE_ATTRIBUTES)
      {
        rex::win::clear_win_errors();
        return false;
      }

      if((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0u)
      {
        return true;
      }

      return false;
    }
    // Copy a directory and its content
    void copy(rsl::string_view src, rsl::string_view dst) // NOLINT(misc-no-recursion)
    {
      const rsl::string full_src = vfs::create_full_path(src);
      const rsl::string full_dst = vfs::create_full_path(dst);

      const rsl::vector<rsl::string> all_files = list_files(full_src);
      const rsl::vector<rsl::string> all_dirs  = list_dirs(full_src);

      create(full_dst);

      for(const rsl::string_view file_entry: all_files)
      {
        const rsl::string rel_path  = path::rel_path(file_entry, full_src);
        const rsl::string entry_dst = path::join(full_dst, rel_path);
        file::copy(file_entry, entry_dst);
      }

      for(const rsl::string_view dir_entry: all_dirs)
      {
        const rsl::string rel_path  = path::rel_path(dir_entry, full_src);
        const rsl::string entry_dst = path::join(full_dst, rel_path);
        create(entry_dst);
        copy(dir_entry, entry_dst);
      }
    }
    // Move/Rename a directory
    void move(rsl::string_view src, rsl::string_view dst)
    {
      const rsl::string full_src = vfs::create_full_path(src);
      const rsl::string full_dst = vfs::create_full_path(dst);

      MoveFileA(full_src.c_str(), full_dst.c_str());
    }
    // List all entries under a directory
    rsl::vector<rsl::string> list_entries(rsl::string_view path)
    {
      WIN32_FIND_DATAA ffd;
      rsl::big_stack_string dir_search(path);
      dir_search += "\\*";
      HANDLE find_handle = FindFirstFileA(path.data(), &ffd);

      if(find_handle == INVALID_HANDLE_VALUE)
      {
        FindClose(find_handle);
        return {};
      }

      rsl::vector<rsl::string> result;
      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        const rsl::string_view name = ffd.cFileName;
        result.push_back(path::join(path, name));
      } while(FindNextFile(find_handle, &ffd) != 0);

      return result;
    }
    // List all directories under a directory
    rsl::vector<rsl::string> list_dirs(rsl::string_view path)
    {
      WIN32_FIND_DATAA ffd;
      rsl::big_stack_string dir_search(path);
      dir_search += "\\*";
      HANDLE find_handle = FindFirstFileA(path.data(), &ffd);

      if(find_handle == INVALID_HANDLE_VALUE)
      {
        FindClose(find_handle);
        return {};
      }

      rsl::vector<rsl::string> result;
      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        const rsl::string_view name     = ffd.cFileName;
        const rsl::string full_filename = path::join(path, name);
        if(exists(full_filename))
        {
          result.push_back(rsl::move(full_filename));
        }
      } while(FindNextFile(find_handle, &ffd) != 0);

      return result;
    }
    // List all files under a directory
    rsl::vector<rsl::string> list_files(rsl::string_view path)
    {
      WIN32_FIND_DATAA ffd {};
      rsl::big_stack_string dir_search(path);
      dir_search += "\\*";
      HANDLE find_handle = FindFirstFileA(dir_search.data(), &ffd);

      if(find_handle == INVALID_HANDLE_VALUE)
      {
        FindClose(find_handle);
        return {};
      }

      rsl::vector<rsl::string> result;
      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        const s32 length = rsl::strlen(ffd.cFileName);
        const rsl::string_view name(ffd.cFileName, length);
        const rsl::string full_filename = path::join(path, name);
        if(file::exists(full_filename))
        {
          result.push_back(rsl::move(full_filename));
        }
      } while(FindNextFileA(find_handle, &ffd) != 0);

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
      if(!file.is_valid())
      {
        return rsl::time_point();
      }

      FILETIME creation_time {};
      GetFileTime(file.get(), &creation_time, nullptr, nullptr);
      const SYSTEMTIME sys_time = rsl::win::to_local_sys_time(creation_time);
      return rsl::timepoint_from_systime(sys_time);
    }
    // Return the access time of a directory
    rsl::time_point access_time(rsl::string_view path)
    {
      const rsl::win::handle file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if(!file.is_valid())
      {
        return rsl::time_point();
      }

      FILETIME access_time {};
      GetFileTime(file.get(), nullptr, &access_time, nullptr);
      const SYSTEMTIME sys_time = rsl::win::to_local_sys_time(access_time);
      return rsl::timepoint_from_systime(sys_time);
    }
    // Return the modification time of a directory
    rsl::time_point modification_time(rsl::string_view path)
    {
      const rsl::win::handle file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if(!file.is_valid())
      {
        return rsl::time_point();
      }

      FILETIME modification_time {};
      GetFileTime(file.get(), nullptr, nullptr, &modification_time);
      const SYSTEMTIME sys_time = rsl::win::to_local_sys_time(modification_time);
      return rsl::timepoint_from_systime(sys_time);
    }
  } // namespace directory
} // namespace rex