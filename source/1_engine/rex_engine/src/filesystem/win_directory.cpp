#include "rex_engine/platform/win/filesystem/win_directory.h"

#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/time/win/win_time_functions.h"

namespace rex
{
  namespace directory
  {
    DEFINE_LOG_CATEGORY(LogDirectory);

    namespace internal
    {
      rsl::win::handle open_file_for_attribs(rsl::string_view path)
      {
        return rsl::win::handle(CreateFileA(path.data(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr));
      }
    } // namespace internal

    // Create a new directory
    Error create(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);
      if(exists(full_path))
      {
        return Error::create_with_log(LogDirectory, "Cannot create directory \"{}\" as it already exists", full_path);
      }

      const bool success = WIN_SUCCESS(CreateDirectoryA(full_path.c_str(), nullptr));

      return success
        ? Error::no_error()
        : Error::create_with_log(LogDirectory, "Failed to create directory at \"{}\"", full_path);
    }
    // Create a directory recursively, creating all sub directories until the leaf dir
    Error create_recursive(rsl::string_view path)
    {
      const rsl::string fullpath = path::abs_path(path);
      const rsl::vector<rsl::string_view> splitted_paths = rsl::split(fullpath, "/\\");

      rsl::string full_path;
      full_path.reserve(fullpath.size());

      for (const rsl::string_view sub_path : splitted_paths)
      {
        full_path += sub_path;
        const bool success = WIN_SUCCESS_IGNORE(CreateDirectoryA(full_path.data(), NULL), ERROR_ALREADY_EXISTS);

        if (!success)
        {
          return Error::create_with_log(LogDirectory, "Failed to create directory at \"{}\"", full_path);
        }

        full_path += g_folder_seps;
      }

      return Error::no_error();
    }
    // Delete a directory
    Error del(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);
      const bool success = RemoveDirectoryA(full_path.c_str());

      return success
        ? Error::no_error()
        : Error::create_with_log(LogDirectory, "Failed to delete directory at \"{}\"", full_path);
    }
    // Return if a directory exists
    bool exists(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);
      const DWORD attribs   = WIN_CALL_IGNORE(GetFileAttributesA(full_path.c_str()), ERROR_FILE_NOT_FOUND);

      if(attribs == INVALID_FILE_ATTRIBUTES)
      {
        return false;
      }

      if((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0u)
      {
        return true;
      }

      return false;
    }
    // Copy a directory and its content
    Error copy(rsl::string_view src, rsl::string_view dst) // NOLINT(misc-no-recursion)
    {
      const rsl::string full_src = path::abs_path(src);
      const rsl::string full_dst = path::abs_path(dst);

      const rsl::vector<rsl::string> all_files = list_files(full_src);
      const rsl::vector<rsl::string> all_dirs  = list_dirs(full_src);

      Error error = create(full_dst);
      if (error)
      {
        return error;
      }

      for(const rsl::string_view file_entry: all_files)
      {
        const rsl::string rel_path  = path::rel_path(file_entry, full_src);
        const rsl::string entry_dst = path::join(full_dst, rel_path);
        error = file::copy(file_entry, entry_dst);
        if (error)
        {
          return error;
        }
      }

      for(const rsl::string_view dir_entry: all_dirs)
      {
        const rsl::string rel_path  = path::rel_path(dir_entry, full_src);
        const rsl::string entry_dst = path::join(full_dst, rel_path);
        error = create(entry_dst);
        if (error)
        {
          return error;
        }
        error = copy(dir_entry, entry_dst);
        if (error)
        {
          return error;
        }
      }

      return Error::no_error();
    }
    // Move/Rename a directory
    Error move(rsl::string_view src, rsl::string_view dst)
    {
      const rsl::string full_src = path::abs_path(src);
      const rsl::string full_dst = path::abs_path(dst);

      const bool success = MoveFileA(full_src.c_str(), full_dst.c_str());

      return success
        ? Error::no_error()
        : Error::create_with_log(LogDirectory, "Failed to move directory from \"{}\" to \"{}\"", full_src, full_dst);
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