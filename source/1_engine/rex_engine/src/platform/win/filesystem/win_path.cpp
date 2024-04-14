#include "rex_engine/platform/win/filesystem/win_path.h"

#include "rex_engine/engine/numeric.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/platform/win/win_com_library.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
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
        auto err = GetLastError();
        HANDLE hfind = WIN_CALL_IGNORE(FindFirstFileA(path.data(), &find_file_data), ERROR_FILE_NOT_FOUND);

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

        err = GetLastError();
        return ReparseTag::None;
      }
    } // namespace internal

    // Returns the current working directory
    rsl::string cwd()
    {
      rsl::medium_stack_string current_dir;
      GetCurrentDirectoryA(current_dir.max_size(), current_dir.data()); // NOLINT(readability-static-accessed-through-instance)
      current_dir.reset_null_termination_offset();
      return rsl::string(current_dir).replace("\\", "/");
    }
    // Returns the path of the current user's temp folder
    rsl::string temp_path()
    {
      rsl::big_stack_string str;
      GetTempPathA(str.max_size(), str.data()); // NOLINT(readability-static-accessed-through-instance)
      str.reset_null_termination_offset();
      return rsl::string(str);
    }
    // For symlinks, returns the path the link points to
    // Otherwise returns the input
    rsl::string real_path(rsl::string_view path)
    {
      // It's a bit tricky to get the real path as there are multiple ways
      // of linking to a the same file (.lnk files, symlinks, hardlinks, junctions)
      rsl::string fullpath = abs_path(path);

      fullpath = rex::path::norm_path(fullpath);

      // If the path doesn't exist, just return its input
      if(!file::exists(fullpath) && !directory::exists(fullpath))
      {
        return fullpath;
      }

      // If the path is a .lnk file, we can read its link
      if(rex::path::extension(fullpath).ends_with(".lnk"))
      {
        rsl::string res = rex::win::com_lib::read_link(fullpath);
        res.replace("\\", "/");
        return res;
      }

      // For any other case, we'll try GetFullPathName
      // this can return an empty path in rare cases
      // If it does, just return the input
      rsl::big_stack_string stack_res;
      GetFullPathNameA(fullpath.data(), fullpath.length(), stack_res.data(), nullptr);
      stack_res.reset_null_termination_offset();

      if(stack_res.empty())
      {
        return fullpath;
      }

      rsl::string res(stack_res);
      res.replace("\\", "/");
      return res;
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
      const SplitResult splitted_origin = split_origin(path);
      SplitRootResult res {};

      // fill in the values
      if(!splitted_origin.head.empty())
      {
        res.drive = splitted_origin.head.substr(0, 2); // split the drive letter and the colon
        res.root  = splitted_origin.head.substr(2, 1);
        res.tail  = splitted_origin.tail;
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

  } // namespace path
} // namespace rex