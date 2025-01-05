#include "rex_engine/platform/win/filesystem/win_directory.h"

#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/time/win/win_time_functions.h"
#include "rex_engine/containers/temp_vector.h"
#include "rex_std/bonus/utility/output_param.h"

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

      void list_entries(rsl::string_view path, Recursive goRecursive, rsl::Out<rsl::vector<rsl::string>> outResult)
      {
        rex::TempHeapScope tmp_heap_scope{};
        WIN32_FIND_DATAA ffd;
        TempString full_path = rex::path::abs_path(path);
        full_path += "\\*";
        HANDLE find_handle = FindFirstFileA(full_path.data(), &ffd);

        if (find_handle == INVALID_HANDLE_VALUE)
        {
          FindClose(find_handle);
          return;
        }

        TempVector<TempString> dirs;
        TempString fullpath;
        do // NOLINT(cppcoreguidelines-avoid-do-while)
        {
          s32 length = rsl::strlen(ffd.cFileName);
          const rsl::string_view name(ffd.cFileName, length);
          if (name == "." || name == "..")
          {
            continue;
          }

          fullpath = path::join(path, name);
          outResult.get().push_back(rsl::string(fullpath));
          if (goRecursive && directory::exists(fullpath))
          {
            dirs.push_back(rsl::move(fullpath));
          }

        } while (FindNextFileA(find_handle, &ffd) != 0);

        // FindNextfile sets the error to ERROR_NO_MORE_FILES
        // if there are no more files found
        // We reset it here to avoid any confusion
        rex::win::clear_win_errors();

        for (rsl::string_view dir : dirs)
        {
          list_entries(dir, goRecursive, outResult);
        }

        return;
      }

    } // namespace internal

    // Create a new directory
    Error create(rsl::string_view path)
    {
      rex::TempHeapScope tmp_heap_scope{};
      const TempString full_path = path::abs_path(path);
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
      rex::TempHeapScope tmp_heap_scope{};
      const TempString fullpath = path::abs_path(path);
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
      rex::TempHeapScope tmp_heap_scope{};
      const TempString full_path = path::abs_path(path);

			rsl::vector<rsl::string> entries = list_entries(full_path);
			if (!entries.empty())
			{
				return Error::create_with_log(LogDirectory, "Failed to delete directory as it wasn't empty. Directory: \"{}\"", full_path);
			}

			const bool success = RemoveDirectoryA(full_path.c_str());

			return success
				? Error::no_error()
				: Error::create_with_log(LogDirectory, "Failed to delete directory at \"{}\"", full_path);
    }
    // Delete a directory recursively, including all files and sub folders
    Error del_recursive(rsl::string_view path)
    {
      rex::TempHeapScope tmp_heap_scope{};
      const TempString full_path = path::abs_path(path);

      Error error = Error::no_error();

      rsl::vector<rsl::string> dirs = list_dirs(full_path);
      for (rsl::string_view dir : dirs)
      {
        error = del_recursive(dir);
        if (error)
        {
          return Error::create_with_log(LogDirectory, "Failed to recursively delete \"{}\"", full_path);
        }
      }

      rsl::vector<rsl::string> files = list_files(full_path);
      for (rsl::string_view file : files)
      {
        error = file::del(file);
        if (error)
        {
          return Error::create_with_log(LogDirectory, "Failed to recursively delete \"{}\"", full_path);
        }
      }

      return directory::del(full_path);
    }
    // Return if a directory exists
    bool exists(rsl::string_view path)
    {
      rex::TempHeapScope tmp_heap_scope{};
      const TempString full_path = path::abs_path(path);

      // It's possible the error returned here is ERROR_FILE_NOT_FOUND or ERROR_PATH_NOT_FOUND
      // because we can't ignore both, we just call it without wrapping it in WIN_CALL
      // and manually reset the windows error if an error has occurred
      const DWORD attribs   = GetFileAttributesA(full_path.c_str());

      if(attribs == INVALID_FILE_ATTRIBUTES)
      {
        win::clear_win_errors();
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
      rex::TempHeapScope tmp_heap_scope{};
      const TempString full_src = path::abs_path(src);
      const TempString full_dst = path::abs_path(dst);

      const rsl::vector<rsl::string> all_files = list_files(full_src);
      const rsl::vector<rsl::string> all_dirs  = list_dirs(full_src);

      Error error = create(full_dst);
      if (error)
      {
        return error;
      }

      for(const rsl::string_view file_entry: all_files)
      {
        const TempString rel_path  = path::rel_path(file_entry, full_src);
        const TempString entry_dst = path::join(full_dst, rel_path);
        error = file::copy(file_entry, entry_dst);
        if (error)
        {
          return error;
        }
      }

      for(const rsl::string_view dir_entry: all_dirs)
      {
        const TempString rel_path  = path::rel_path(dir_entry, full_src);
        const TempString entry_dst = path::join(full_dst, rel_path);
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
      rex::TempHeapScope tmp_heap_scope{};
      const TempString full_src = path::abs_path(src);
      const TempString full_dst = path::abs_path(dst);

      const bool success = MoveFileA(full_src.c_str(), full_dst.c_str());

      return success
        ? Error::no_error()
        : Error::create_with_log(LogDirectory, "Failed to move directory from \"{}\" to \"{}\"", full_src, full_dst);
    }

    // Returns true if the directory is empty, 
    // as in, it has no files or directories
    bool is_empty(rsl::string_view path)
    {
      rex::TempHeapScope tmp_heap_scope{};
      WIN32_FIND_DATAA ffd;
      TempString full_path = rex::path::abs_path(path);
      full_path += "\\*";
      HANDLE find_handle = FindFirstFileA(full_path.data(), &ffd);

      // If the path doesn't exist, we pretend that the directory is empty
      if (find_handle == INVALID_HANDLE_VALUE)
      {
        FindClose(find_handle);
        return true;
      }

      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        s32 length = rsl::strlen(ffd.cFileName);
        const rsl::string_view name(ffd.cFileName, length);
        if (name == "." || name == "..")
        {
          continue;
        }

        return false;
      } while (FindNextFileA(find_handle, &ffd) != 0);

      // FindNextfile sets the error to ERROR_NO_MORE_FILES
      // if there are no more files found
      // We reset it here to avoid any confusion
      rex::win::clear_win_errors();

      return true;
    }

    // List the number of entries under a directory
    s32 num_entries(rsl::string_view path, Recursive goRecursive)
    {
      rex::TempHeapScope tmp_heap_scope{};
      WIN32_FIND_DATAA ffd;
      TempString full_path = rex::path::abs_path(path);
      full_path += "\\*";
      HANDLE find_handle = FindFirstFileA(full_path.data(), &ffd);

      if (find_handle == INVALID_HANDLE_VALUE)
      {
        FindClose(find_handle);
        return {};
      }

      s32 num_entries = 0;

      TempVector<TempString> dirs;
      TempString fullpath;
      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        s32 length = rsl::strlen(ffd.cFileName);
        const rsl::string_view name(ffd.cFileName, length);
        if (name == "." || name == "..")
        {
          continue;
        }

        fullpath = path::join(path, name);
        if (goRecursive && directory::exists(fullpath))
        {
          dirs.push_back(rsl::move(fullpath));
        }

        ++num_entries;

      } while (FindNextFileA(find_handle, &ffd) != 0);

      // FindNextfile sets the error to ERROR_NO_MORE_FILES
      // if there are no more files found
      // We reset it here to avoid any confusion
      rex::win::clear_win_errors();

      for (rsl::string_view dir : dirs)
      {
        num_entries += directory::num_entries(dir, goRecursive);
      }

      return num_entries;
    }
    // List the number of directories under a directory
    s32 num_dirs(rsl::string_view path)
    {
      rex::TempHeapScope tmp_heap_scope{};
      WIN32_FIND_DATAA ffd;
      TempString full_path = rex::path::abs_path(path);
      full_path += "\\*";
      HANDLE find_handle = FindFirstFileA(full_path.data(), &ffd);

      if (find_handle == INVALID_HANDLE_VALUE)
      {
        FindClose(find_handle);
        return {};
      }

      s32 num_dirs = 0;
      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        s32 length = rsl::strlen(ffd.cFileName);
        const rsl::string_view name(ffd.cFileName, length);
        const TempString full_filename = path::join(path, name);
        if (exists(full_filename) && name != "." && name != "..")
        {
          ++num_dirs;
        }
      } while (FindNextFileA(find_handle, &ffd) != 0);

      // FindNextfile sets the error to ERROR_NO_MORE_FILES
      // if there are no more files found
      // We reset it here to avoid any confusion
      rex::win::clear_win_errors();

      return num_dirs;
    }
    // List the number of files under a directory
    s32 num_files(rsl::string_view path)
    {
      rex::TempHeapScope tmp_heap_scope{};
      WIN32_FIND_DATAA ffd{};
      TempString full_path = rex::path::abs_path(path);
      full_path += "\\*";
      HANDLE find_handle = FindFirstFileA(full_path.data(), &ffd);

      if (find_handle == INVALID_HANDLE_VALUE)
      {
        FindClose(find_handle);
        return {};
      }

      s32 num_files = 0;
      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        const s32 length = rsl::strlen(ffd.cFileName);
        const rsl::string_view name(ffd.cFileName, length);
        TempString full_filename = path::join(path, name);
        if (file::exists(full_filename))
        {
          ++num_files;
        }
      } while (FindNextFileA(find_handle, &ffd) != 0);

      // FindNextfile sets the error to ERROR_NO_MORE_FILES
      // if there are no more files found
      // We reset it here to avoid any confusion
      rex::win::clear_win_errors();

      return num_files;
    }

    // List all entries under a directory
    rsl::vector<rsl::string> list_entries(rsl::string_view path, Recursive goRecursive)
    {
      rsl::vector<rsl::string> result;

      // We do this through a function taking in a output parameter
      // as it can massively improve performance due to less copying of strings
      // and less allocations as well
      internal::list_entries(path, goRecursive, rsl::Out(result));

      return result;
    }
    // List all directories under a directory
    rsl::vector<rsl::string> list_dirs(rsl::string_view path)
    {
      rex::TempHeapScope tmp_heap_scope{};
      WIN32_FIND_DATAA ffd;
      TempString full_path = rex::path::abs_path(path);
      full_path += "\\*";
      HANDLE find_handle = FindFirstFileA(full_path.data(), &ffd);

      if(find_handle == INVALID_HANDLE_VALUE)
      {
        FindClose(find_handle);
        return {};
      }

      rsl::vector<rsl::string> result;

      s32 num_dirs = directory::num_dirs(full_path);
      result.reserve(num_dirs);

      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        s32 length = rsl::strlen(ffd.cFileName);
        const rsl::string_view name(ffd.cFileName, length);
        const TempString full_filename = path::join(path, name);
        if(exists(full_filename) && name != "." && name != "..")
        {
          result.push_back(rsl::string(full_filename));
        }
      } while(FindNextFileA(find_handle, &ffd) != 0);

      // FindNextfile sets the error to ERROR_NO_MORE_FILES
      // if there are no more files found
      // We reset it here to avoid any confusion
      rex::win::clear_win_errors();

      return result;
    }
    // List all files under a directory
    rsl::vector<rsl::string> list_files(rsl::string_view path)
    {
      rex::TempHeapScope tmp_heap_scope{};
      WIN32_FIND_DATAA ffd {};
      TempString full_path = rex::path::abs_path(path);
      full_path += "\\*";
      HANDLE find_handle = FindFirstFileA(full_path.data(), &ffd);

      if(find_handle == INVALID_HANDLE_VALUE)
      {
        FindClose(find_handle);
        return {};
      }

      rsl::vector<rsl::string> result;

      s32 num_files = directory::num_files(full_path);
      result.reserve(num_files);

      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        const s32 length = rsl::strlen(ffd.cFileName);
        const rsl::string_view name(ffd.cFileName, length);
        TempString full_filename = path::join(path, name);
        if(file::exists(full_filename))
        {
          result.push_back(rsl::string(full_filename));
        }
      } while(FindNextFileA(find_handle, &ffd) != 0);

      // FindNextfile sets the error to ERROR_NO_MORE_FILES
      // if there are no more files found
      // We reset it here to avoid any confusion
      rex::win::clear_win_errors();

      return result;
    }

    // Return the creation time of a directory
    rsl::time_point creation_time(rsl::string_view path)
    {
      rex::TempHeapScope tmp_heap_scope{};
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
      rex::TempHeapScope tmp_heap_scope{};
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
      rex::TempHeapScope tmp_heap_scope{};
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