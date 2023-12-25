#include "rex_engine/platform/win/filesystem/win_path.h"

#include "rex_engine/engine/numeric.h"
#include "rex_engine/platform/win/win_com_library.h"
#include "rex_std/algorithm.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/ctype.h"
#include "rex_std/format.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/time/win/win_time_functions.h"

// The current implementation of this namespace is Windows only

//#define NOMINMAX
//#include <Windows.h>
//#include <cstdlib>
//#include <processenv.h>

namespace rex
{
  namespace path
  {
    constexpr char8 g_seperation_char = '/';

    namespace internal
    {
      // concat the arg to the string in filepath format
      void join_impl(rsl::string& str, rsl::string_view arg)
      {
        if (arg.empty())
        {
          return;
        }

        str += arg;

        if (!str.ends_with(seperation_char()))
        {
          str += seperation_char();
        }
      }

      // returns the position of where the extension of the path starts,
      // if there is any
      card32 extension_start(rsl::string_view path)
      {
        // because it's possible to have a path like this
        // ./relative/path/file.txt
        // we can need to scan for the first dot 
        // after the first slash
        auto pos = path.find_last_of("/\\");
        auto filename = pos != path.npos()
          ? path.substr(pos + 1)
          : path;

        // current dir and parent dir "filenames" don't have extensions
        if (filename == "." || filename == "..")
        {
          return path.npos();
        }

        auto ext_start = filename.find_first_of('.');

        // make sure we convert the pos in the filename back to the pos
        // of the total input path
        return ext_start != filename.npos()
          ? static_cast<card32>(&filename[ext_start] - path.data())
          : path.npos();
      }

      // Fills a string with a number of random characters
      // This is useful for creating random filenames and directories
      void fill_with_random_chars(rsl::string& str, card32 numCharsToFill)
      {
        rsl::small_stack_string chars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
        for (card32 i = 0; i < numCharsToFill; ++i)
        {
          card32 random_idx = std::rand() % chars.length();
          str += chars[random_idx];
        }
      }

      enum class ReparseTag
      {
        None,
        Junction,
        Symlink,
      };

      ReparseTag get_reparse_tag(rsl::string_view path)
      {
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFileA(path.data(), &findFileData);

        if (hFind == INVALID_HANDLE_VALUE)
        {
          return ReparseTag::None; // Default to Junction in case of an error
        }

        FindClose(hFind);

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
        {
          if (findFileData.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT)
          {
            return ReparseTag::Junction;
          }
          else if (findFileData.dwReserved0 == IO_REPARSE_TAG_SYMLINK)
          {
            return ReparseTag::Symlink;
          }
        }

        return ReparseTag::None;
      }

      rsl::win::handle open_file_for_attribs(rsl::string_view path)
      {
        rsl::win::handle file(CreateFileA(path.data(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0));

        return file;
      }
    } // namespace internal

    // returns the seperation char for paths
    char8 seperation_char()
    {
      return g_seperation_char;
    }

    // Changes the extension of a path string_view
    // If extension argument is empty, the extension is removed
    // if the path doesn't have an extension, the extension specified gets appended
    rsl::string change_extension(rsl::string_view path, rsl::string_view extension)
    {
      SplitResult split_res = split_ext(path);

      // use the extension split to store the path without the extension
      rsl::string res(split_res.head);

      // Add a dot if the provided one doesn't have one
      if (!extension.empty() && !extension.starts_with('.'))
      {
        res += '.';
      }

      // Add the new extension
      res += extension;

      return res;
    }
    // Returns the directory path of the given path
    rsl::string_view dir_name(rsl::string_view path)
    {
      SplitResult split_res = split(path);
      return split_res.head;
    }
    // Returns the extension of the given path
    rsl::string_view extension(rsl::string_view path)
    {
      SplitResult split_res = split_ext(path);
      return split_res.tail;
    }
    // Returns the filename of the given path
    rsl::string_view filename(rsl::string_view path)
    {
      SplitResult split_res = split(path);
      return split_res.tail;
    }
    // Returns the filename of the given path without its extension
    rsl::string_view stem(rsl::string_view path)
    {
      // get the filename of the path
      rsl::string_view file_name = filename(path);

      // get the position of the extension, if there is one
      card32 extension_pos = internal::extension_start(file_name);
      card32 count = extension_pos != -1 ? extension_pos : file_name.length();

      // return the substring of the filename, without the extension
      return file_name.substr(0, count);
    }
    // Returns the absolute path for the given path
    rsl::string abs_path(rsl::string_view path)
    {
      // If the path is already absolute, just return it
      if (is_absolute(path))
      {
        return rsl::string(path);
      }

      // Get the current working directory and prepend it to the path
      rsl::medium_stack_string current_dir;
      GetCurrentDirectoryA(current_dir.max_size(), current_dir.data());
      current_dir.reset_null_termination_offset();
      rsl::string res = path::join(current_dir, path);
      return res.replace("\\", "/");
    }
    // Returns the root directory path of the given path
    rsl::string_view path_root(rsl::string_view path)
    {
      // if the path is an absolute path, return the path root
      if (is_absolute(path))
      {
        return path.substr(0, path.find_first_of("/\\"));
      }

      // otherwise return an empty path
      return "";
    }
    // Returns the current working directory
    rsl::string cwd()
    {
      rsl::medium_stack_string current_dir;
      GetCurrentDirectoryA(current_dir.max_size(), current_dir.data());
      current_dir.reset_null_termination_offset();
      return rsl::string(current_dir).replace("\\", "/");
    }
    // Returns a random directory, but doesn't create it
    rsl::string random_dir()
    {
      // create a directory name of 8 random characters
      card32 num_dirname_chars = 8;
      rsl::string result;

      do
      {
        // clear the result first before adding to it
        result.clear();

        // create a stem of 8 random characters
        internal::fill_with_random_chars(result, num_dirname_chars);

      } while (path::exists(result));

      return result;
    }
    // Returns a random filename, but doesn't create it
    rsl::string random_filename()
    {
      card32 num_stem_chars = 8;
      card32 num_ext_chars = 3;
      rsl::string result;

      do
      {
        // clear the result first before adding to it
        result.clear();

        // create a stem of 8 random characters
        internal::fill_with_random_chars(result, num_stem_chars);
        result += '.';

        // create an extension of 3 random characters
        internal::fill_with_random_chars(result, num_ext_chars);

      } while (path::exists(result));

      return result;
    }
    // Returns the path of the current user's temp folder
    rsl::string temp_path()
    {
      rsl::big_stack_string str;
      GetTempPathA(str.max_size(), str.data());
      str.reset_null_termination_offset();
      return rsl::string(str);
    }
    // Returns the longest common sub-path of each pathname in the sequence
    rsl::string_view common_path(const rsl::vector<rsl::string_view>& paths)
    {
      // if no paths are given, just return an empty path
      if (paths.empty())
      {
        return "";
      }

      // split the first path into different path components
      rsl::vector<rsl::string_view> splitted = rsl::split(paths.front(), "/\\");
      auto furthest_path_component_it = splitted.cbegin();

      // iterate over the other paths
      for (card32 i = 1; i < paths.size(); ++i)
      {
        rsl::string_view path = paths[i];

        // split the path into different path components
        rsl::vector<rsl::string_view> splitted_path = rsl::split(path, "/\\");

        // find the first mismatch with the first path in the list
        auto res = rsl::mismatch(splitted.cbegin(), splitted.cend(), splitted_path.cbegin(), splitted_path.cend());

        // if none are equal, return an empty path
        if (res.lhs_it == splitted.cbegin())
        {
          return "";
        }

        // otherwise store the max iterator where the mismatch occurred on a previous run
        furthest_path_component_it = (rsl::max)(furthest_path_component_it, res.lhs_it);
      }

      // store the index of the path component to figure out where the common path ends
      card32 path_component_idx = rsl::distance(splitted.cbegin(), furthest_path_component_it);

      // count the path components, so we know where out substring should end
      rsl::string_view first_path = paths.front();
      card32 pos = 0;
      for (card32 i = 0; i < path_component_idx; ++i)
      {
        pos = first_path.find_first_of("/\\", pos);
        ++pos;
      }

      // return the substring, this is the common path between all paths given
      return first_path.substr(0, pos);
    }
    // For symlinks, returns the path the link points to
    // Otherwise returns the input
    rsl::string real_path(rsl::string_view path)
    {
      // It's a bit tricky to get the real path as there are multiple ways 
      // of linking to a the same file (.lnk files, symlinks, hardlinks, junctions)

      // If the path doesn't exist, just return its input
      if (!rex::path::exists(path))
      {
        return rsl::string(path);
      }

      // If the path is a .lnk file, we can read its link
      if (rex::path::extension(path).ends_with(".lnk"))
      {
        rsl::string res = rex::win::com_library().read_link(path);
        res.replace("\\", "/");
        return res;
      }

      // For any other case, we'll try GetFullPathName
      // this can return an empty path in rare cases
      // If it does, just return the input
      rsl::big_stack_string stack_res;
      GetFullPathNameA(path.data(), path.length(), stack_res.data(), NULL);
      stack_res.reset_null_termination_offset();

      if (stack_res.empty())
      {
        return rex::path::norm_path(path);
      }

      rsl::string res(stack_res);
      res.replace("\\", "/");
      return res;
    }
    // Normalizes the path, removing redundant dots for current and parent directories
    // Converts forward slashes to backward slashes
    rsl::string norm_path(rsl::string_view path)
    {
      rsl::vector<rsl::string_view> splitted_path = rsl::split(path, "/\\");
      rsl::vector<rsl::string_view> norm_splitted(rsl::Capacity(splitted_path.size()));
      rsl::string res;

      // loop over each path component in the given path
      for (rsl::string_view path_comp : splitted_path)
      {
        // if the "current dir" token is found, we skip it
        if (path_comp == ".")
        {
          continue;
        }

        // if the parent dir is found, we remove the last added element
        // unless that's the parent dir token.
        // if the parent dir is found, it means there are no known parents anymore
        // then we add the parent dir to the normalized path components
        if (path_comp == "..")
        {
          if (!norm_splitted.empty() && norm_splitted.back() != "..")
          {
            norm_splitted.pop_back();
            continue;
          }
        }

        // in any other case, we add the path component to the list
        norm_splitted.push_back(path_comp);
      }

      // join everything back together and return the result
      return rsl::join(norm_splitted, rsl::string_view(&g_seperation_char, 1)).as_string();
    }
    // Returns a relative path to path, starting from the start directory
    rsl::string rel_path(rsl::string_view path, rsl::string_view start)
    {
      rsl::string norm_path = path::norm_path(path);
      rsl::string norm_start = path::norm_path(start);

      if (norm_path.empty() && norm_start.empty())
      {
        return rsl::string("");
      }

      rsl::vector<rsl::string_view> splitted_path = rsl::split(norm_path, rsl::string_view(&g_seperation_char, 1));
      rsl::vector<rsl::string_view> splitted_start = rsl::split(norm_start, rsl::string_view(&g_seperation_char, 1));

      auto res = rsl::mismatch(splitted_path.cbegin(), splitted_path.cend(), splitted_start.cbegin(), splitted_start.cend());

      // If we need to go up in the filesystem
      // Check how many "parent dir" tokens we need to add
      // Eg. target: "dir", start: "path"
      // result: "../dir"
      rsl::string result;
      if (res.lhs_it == splitted_path.cbegin())
      {
        card32 num_parent_dir_tokens = splitted_start.size();
        for (card32 i = 0; i < num_parent_dir_tokens; ++i)
        {
          result = path::join(result, "..");
        }
      }

      return path::join(result, rsl::join(res.lhs_it, splitted_path.cend(), rsl::string_view(&g_seperation_char, 1)).as_string());
    }
    // Returns the latest access time of the file or directory at the given path
    card64 get_access_time(rsl::string_view path)
    {
      rsl::win::handle file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if (!file.is_valid())
      {
        return 0;
      }

      FILETIME access_time{};
      GetFileTime(file.get(), NULL, &access_time, NULL);
      return rsl::win::to_integer(access_time);
    }
    // Returns the modification time of the file or directory at the given path
    card64 get_modification_time(rsl::string_view path)
    {
      rsl::win::handle file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if (!file.is_valid())
      {
        return 0;
      }

      FILETIME modification_time{};
      GetFileTime(file.get(), NULL, NULL, &modification_time);
      return rsl::win::to_integer(modification_time);
    }
    // Returns the creation time of the file or directory at the given path
    card64 get_creation_time(rsl::string_view path)
    {
      rsl::win::handle file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if (!file.is_valid())
      {
        return 0;
      }

      FILETIME creation_time{};
      GetFileTime(file.get(), &creation_time, NULL, NULL);
      return rsl::win::to_integer(creation_time);
    }
    // Returns the creation time of the file or directory at the given path
    card64 get_file_size(rsl::string_view path)
    {
      rsl::win::handle file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we simply return the input
      if (!file.is_valid())
      {
        return -1;
      }

      DWORD high_word = 0;
      DWORD low_word = GetFileSize(file.get(), &high_word);
      return rex::merge_int32_to_int64(high_word, low_word);
    }
    // Returns if the given path has an extension
    bool has_extension(rsl::string_view path)
    {
      if (path == ".")
      {
        return false;
      }

      if (path == "..")
      {
        return false;
      }

      auto start = internal::extension_start(path);
      return start != rsl::string_view::npos() && start != path.size() - 1;
    }
    // Returns if the path exists
    bool exists(rsl::string_view path)
    {
      DWORD attribs = GetFileAttributesA(path.data());
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

      if (path.front() == '/' || path.front() == '\\')
      {
        return true;
      }

      if (!rsl::is_alpha(path.front()))
      {
        return false;
      }

      if (path[1] != ':')
      {
        return false;
      }

      if (path[2] != '/' && path[2] != '\\')
      {
        return false;
      }

      return true;
    }
    // Returns if the given path is a relative path
    bool is_relative(rsl::string_view path)
    {
      if (path.empty())
      {
        return false;
      }

      if (path == ".")
      {
        return false;
      }

      if (path == "..")
      {
        return false;
      }

      return !is_absolute(path);
    }
    // Returns true if the given path points to a file
    bool is_file(rsl::string_view path)
    {
      DWORD attribs = GetFileAttributesA(path.data());

      if (attribs == INVALID_FILE_ATTRIBUTES)
      {
        return false;
      }

      if (attribs & FILE_ATTRIBUTE_DIRECTORY)
      {
        return false;
      }

      return true;
    }
    // Returns true if the given path points to a directory
    bool is_dir(rsl::string_view path)
    {
      DWORD attribs = GetFileAttributesA(path.data());

      if (attribs == INVALID_FILE_ATTRIBUTES)
      {
        return false;
      }

      if (attribs & FILE_ATTRIBUTE_DIRECTORY)
      {
        return true;
      }

      return false;
    }
    // Returns true if the given path points to a junction
    bool is_junction(rsl::string_view path)
    {
      internal::ReparseTag tag = internal::get_reparse_tag(path);
      return tag == internal::ReparseTag::Junction;
    }
    // Returns true if the given path points to a symlink
    bool is_link(rsl::string_view path)
    {
      internal::ReparseTag tag = internal::get_reparse_tag(path);
      return tag == internal::ReparseTag::Symlink;
    }
    // Returns true if 2 paths point to the same file
    bool same_path(rsl::string_view path1, rsl::string_view path2)
    {
      // simply convert the files into their actual files on disk
      // then do a string wise comparison
      rsl::string real_path1 = real_path(path1);
      rsl::string real_path2 = real_path(path2);
      return real_path1 == real_path2;
    }
    // Splits the path into a head and a tail
    // the tail is the last pathname component
    // the head is everything leading up to that
    SplitResult split(rsl::string_view path)
    {
      SplitResult res{};

      // get the last slash position
      card32 pos = path.find_last_of("/\\");

      // fill in the values
      if (pos != path.npos())
      {
        res.head = path.substr(0, pos);
        res.tail = path.substr(pos + 1);
      }
      else
      {
        res.tail = path;
      }

      // return the result
      return res;
    }
    // Splits the path into a head and a tail
    // the head is either the mount point or an empty string
    // the tail is everything else
    SplitResult split_drive(rsl::string_view path)
    {
      SplitResult res{};

      if (path.empty())
      {
        return res;
      }

      // get the last slash position
      card32 slash_pos = path.find(":/");
      card32 backslash_pos = path.find(":\\");

      card32 used_pos = slash_pos != -1 && backslash_pos != -1 ? (rsl::min)(slash_pos, backslash_pos)  // if for any reason the path has the root tokens twice, return the first
        : (rsl::max)(slash_pos, backslash_pos); // in other case, where there's only one, use the one that's found
// pos will point to where ':' is found
// the head should be "<drive letter>:"
// the tail should be everything afterwards, excluding the
// slash seperator of the drive

// fill in the values
      if (used_pos != path.npos())
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
    // Splits the path into a head and a tail
    // the head is the directory and the stem
    // the tail is the extension
    SplitResult split_ext(rsl::string_view path)
    {
      SplitResult res{};

      // get the extension start position
      card32 ext_start = internal::extension_start(path);

      // fill in the values
      if (ext_start != path.npos())
      {
        res.head = path.substr(0, ext_start);
        res.tail = path.substr(ext_start);
      }
      else
      {
        res.head = path;
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
      SplitResult splitted_drive = split_drive(path);
      SplitRootResult res{};

      // fill in the values
      if (!splitted_drive.head.empty())
      {
        res.drive = splitted_drive.head.substr(0, 2); // split the drive letter and the colon
        res.root = splitted_drive.head.substr(2, 1);
        res.tail = splitted_drive.tail;
      }
      else if (path.starts_with('/') || path.starts_with('\\'))
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