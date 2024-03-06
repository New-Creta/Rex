#include "rex_engine/platform/win/filesystem/win_path.h"

#include "rex_engine/engine/numeric.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/platform/win/win_com_library.h"
#include "rex_std/algorithm.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/time/win/win_time_functions.h"
#include "rex_std/format.h"
#include "rex_std/wctype.h"

// The current implementation of this namespace is Windows only

#define NOMINMAX
#include <Windows.h>
#include <cstdlib>
#include <processenv.h>
#include <string>

namespace rex
{
  namespace path
  {
    namespace internal
    {
      enum class ReparseTag
      {
        None,
        Junction,
        Symlink,
      };

      ReparseTag get_reparse_tag(rsl::string_view path)
      {
        WIN32_FIND_DATAA find_file_data;
        HANDLE hfind = FindFirstFileA(path.data(), &find_file_data);

        if(hfind == INVALID_HANDLE_VALUE)
        {
          return ReparseTag::None; // Default to Junction in case of an error
        }

        FindClose(hfind);

        if((find_file_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0u)
        {
          if(find_file_data.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT)
          {
            return ReparseTag::Junction;
          }
          else if(find_file_data.dwReserved0 == IO_REPARSE_TAG_SYMLINK)
          {
            return ReparseTag::Symlink;
          }
        }

        return ReparseTag::None;
      }

      rsl::win::handle open_file_for_attribs(rsl::string_view path)
      {
        rsl::win::handle file(CreateFileA(path.data(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr));

        return file;
      }
    } // namespace internal

    // Returns the current working directory
    rsl::string cwd()
    {
      rsl::medium_stack_string current_dir;
      GetCurrentDirectoryA(current_dir.max_size(), current_dir.data());
      current_dir.reset_null_termination_offset();
      return rsl::string(current_dir).replace("\\", "/");
    }
    // Returns the path of the current user's temp folder
    rsl::string temp_path()
    {
      rsl::big_stack_string str;
      GetTempPathA(str.max_size(), str.data());
      str.reset_null_termination_offset();
      return rsl::string(str);
    }
    // For symlinks, returns the path the link points to
    // Otherwise returns the input
    rsl::string real_path(rsl::string_view path)
    {
      // It's a bit tricky to get the real path as there are multiple ways
      // of linking to a the same file (.lnk files, symlinks, hardlinks, junctions)

      // If the path doesn't exist, just return its input
      if(!rex::path::exists(path))
      {
        return rsl::string(path);
      }

      // If the path is a .lnk file, we can read its link
      if(rex::path::extension(path).ends_with(".lnk"))
      {
        rsl::string res = rex::win::com_library().read_link(path);
        res.replace("\\", "/");
        return res;
      }

      // For any other case, we'll try GetFullPathName
      // this can return an empty path in rare cases
      // If it does, just return the input
      rsl::big_stack_string stack_res;
      GetFullPathNameA(path.data(), path.length(), stack_res.data(), nullptr);
      stack_res.reset_null_termination_offset();

      if(stack_res.empty())
      {
        return rex::path::norm_path(path);
      }

      rsl::string res(stack_res);
      res.replace("\\", "/");
      return res;
    }
    // Returns the latest access time of the file or directory at the given path
    card64 get_access_time(rsl::string_view path)
    {
      rsl::win::handle const file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if(!file.is_valid())
      {
        return 0;
      }

      FILETIME access_time {};
      GetFileTime(file.get(), nullptr, &access_time, nullptr);
      return rsl::win::to_integer(access_time);
    }
    // Returns the modification time of the file or directory at the given path
    card64 get_modification_time(rsl::string_view path)
    {
      rsl::win::handle const file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if(!file.is_valid())
      {
        return 0;
      }

      FILETIME modification_time {};
      GetFileTime(file.get(), nullptr, nullptr, &modification_time);
      return rsl::win::to_integer(modification_time);
    }
    // Returns the creation time of the file or directory at the given path
    card64 get_creation_time(rsl::string_view path)
    {
      rsl::win::handle const file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if(!file.is_valid())
      {
        return 0;
      }

      FILETIME creation_time {};
      GetFileTime(file.get(), &creation_time, nullptr, nullptr);
      return rsl::win::to_integer(creation_time);
    }
    // Returns the creation time of the file or directory at the given path
    card64 get_file_size(rsl::string_view path)
    {
      rsl::win::handle const file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we simply return the input
      if(!file.is_valid())
      {
        return -1;
      }

      DWORD high_word      = 0;
      DWORD const low_word = GetFileSize(file.get(), &high_word);
      return rex::merge_int32_to_int64(high_word, low_word);
    }
    // Returns if the path exists
    bool exists(rsl::string_view path)
    {
      DWORD const attribs = GetFileAttributesA(path.data());
      return attribs != INVALID_FILE_ATTRIBUTES;
    }
    // Returns if the given path is an absolute path
    bool is_absolute(rsl::string_view path)
    {
      // absolute paths
      // C:\foo - yes
      // \foo - yes
      // /foo - yes
      // anything else - no

      if(path.front() == '/' || path.front() == '\\')
      {
        return true;
      }

      if(!rsl::is_alpha(path.front()))
      {
        return false;
      }

      if(path[1] != ':')
      {
        return false;
      }

      if(path[2] != '/' && path[2] != '\\')
      {
        return false;
      }

      return true;
    }
    // Returns true if the given path points to a file
    bool is_file(rsl::string_view path)
    {
      DWORD const attribs = GetFileAttributesA(path.data());

      if(attribs == INVALID_FILE_ATTRIBUTES)
      {
        return false;
      }

      if((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0u)
      {
        return false;
      }

      return true;
    }
    // Returns true if the given path points to a directory
    bool is_dir(rsl::string_view path)
    {
      DWORD const attribs = GetFileAttributesA(path.data());

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
    // Returns true if the given path points to a junction
    bool is_junction(rsl::string_view path)
    {
      internal::ReparseTag const tag = internal::get_reparse_tag(path);
      return tag == internal::ReparseTag::Junction;
    }
    // Returns true if the given path points to a symlink
    bool is_link(rsl::string_view path)
    {
      internal::ReparseTag const tag = internal::get_reparse_tag(path);
      return tag == internal::ReparseTag::Symlink;
    }
    // Splits the path into a head and a tail
    // the head is either the mount point or an empty string
    // the tail is everything else
    SplitResult split_origin(rsl::string_view path)
    {
      SplitResult res {};

      if(path.empty())
      {
        return res;
      }

      // get the last slash position
      const card32 slash_pos     = path.find(":/");
      const card32 backslash_pos = path.find(":\\");

      const card32 used_pos = slash_pos != -1 && backslash_pos != -1 ? (rsl::min)(slash_pos, backslash_pos)  // if for any reason the path has the root tokens twice, return the first
                                                                     : (rsl::max)(slash_pos, backslash_pos); // in other case, where there's only one, use the one that's found

      // pos will point to where ':' is found
      // the head should be "<drive letter>:"
      // the tail should be everything afterwards, excluding the
      // slash seperator of the drive

      // fill in the values
      if(used_pos != rsl::string_view::npos())
      {
        res.head = path.substr(0, used_pos + 1);
        res.tail = path.substr(used_pos + 2);
      }
      else
      {
        res.tail = path;
      }

      // return the result
      return res;
    }
    // Split the path into 3 components
    // drive - root - tail
    // drive: mounting point
    // root: string of separators after the drive
    // tail: everything after the root
    // eg: c:/Users/Sam (Windows)
    // drive: C:
    // root: /
    // tail: Users/Sam
    SplitRootResult split_root(rsl::string_view path)
    {
      // use a split drive result
      const SplitResult splitted_drive = split_origin(path);
      SplitRootResult res {};

      // fill in the values
      if(!splitted_drive.head.empty())
      {
        res.drive = splitted_drive.head.substr(0, 2); // split the drive letter and the colon
        res.root  = splitted_drive.head.substr(2, 1);
        res.tail  = splitted_drive.tail;
      }
      else if(path.starts_with('/') || path.starts_with('\\'))
      {
        res.root = path.substr(0, 1);
        res.tail = path.substr(1);
      }
      else
      {
        res.tail = path;
      }

      // return the result
      return res;
    }

    // Returns a list of all files and directories in path
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

    // Returns a list of all directories in path
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
        if (path::is_dir(full_filename))
        {
          result.push_back(rsl::move(full_filename));
        }
      } while (FindNextFile(hFind, &ffd) != 0);

      return result;
    }

    // Returns a list of all files in path
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
        if (path::is_file(full_filename))
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

  } // namespace path
} // namespace rex