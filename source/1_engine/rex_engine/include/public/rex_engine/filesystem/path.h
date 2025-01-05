#pragma once

#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"
#include "rex_std/vector.h"
#include "rex_std/bonus/utility.h"
#include "rex_engine/engine/types.h"

namespace rex
{
  namespace path
  {
    struct SplitResult
    {
      rsl::string_view head;
      rsl::string_view tail;
    };
    // Splits the path into a head and a tail
    // the tail is the last pathname component
    // the head is everything leading up to that
    SplitResult split(rsl::string_view path);
    // Splits the path into a head and a tail
    // the head is the directory and the stem
    // the tail is the extension
    SplitResult split_ext(rsl::string_view path);
    struct SplitRootResult
    {
      rsl::string_view drive;
      rsl::string_view root;
      rsl::string_view tail;
    };

    // -------------------------------------------------------------------------
    // THESE FUNCTIONS ARE REQUIRED TO BE IMPLEMENTED BY PLATFORM SPECIFIC CODE
    // -------------------------------------------------------------------------
    
    // Returns the current working directory
    rsl::string cwd();

    // Sets a new working directory and returns the old one
    // A valid and existing path is expected or an assert is raised
    rsl::string set_cwd(rsl::string_view dir);

    // Returns the path of the current user's temp folder
    rsl::string temp_path();

    // For symlinks, returns the path the link points to
    // Otherwise returns the input
    rsl::string real_path(rsl::string_view path);

    // Returns if the given path is an absolute path
    bool is_absolute(rsl::string_view path);

    // Returns if a file is under a certain directory
    bool is_under_dir(rsl::string_view path, rsl::string_view dir);

    // Returns true if the given path points to a junction
    bool is_junction(rsl::string_view path);

    // Returns true if the given path points to a symlink
    bool is_link(rsl::string_view path);

    // Splits the path into a head and a tail
    // the head is either the mount point or an empty string
    // the tail is everything else
    SplitResult split_origin(rsl::string_view path);

    // Split the path into 3 components
    // drive - root - tail
    // drive: mounting point
    // root: string of separators after the drive
    // tail: everything after the root
    // eg: c:/Users/Sam (Windows)
    // drive: C:
    // root: /
    // tail: Users/Sam
    SplitRootResult split_root(rsl::string_view path);

    // Returns true if absolute paths on this platform have a drive letter
    bool abs_needs_drive();

    // -------------------------------------------------------------------------
    // END OF PLATFORM SPECIFIC FUNCTIONS
    // -------------------------------------------------------------------------

    namespace internal
    {
      void join_string_view(rsl::string& str, rsl::string_view arg);
      template <typename Enum, rsl::enable_if_t<rsl::is_enum_v<Enum>, bool> = true>
      void join_enum(rsl::string& str, Enum e)
      {
        rsl::string_view enum_name = rsl::enum_refl::enum_name(e);
        join_impl(str, enum_name);
      }
      template <typename PathLikeType>
      void join_path_like(rsl::string& str, PathLikeType&& arg)
      {
        join_string_view(str, rsl::string_view(rsl::forward<PathLikeType>(arg))); // NOLINT(google-readability-casting)
      }

      template <typename PathLikeType>
      void join_impl(rsl::string& str, PathLikeType&& firstArg)
      {
        if constexpr (rsl::is_same_v<rsl::string_view, PathLikeType>)
        {
          join_string_view(str, firstArg);
        }
        else if constexpr (rsl::is_enum_v<PathLikeType>)
        {
          join_enum(str, firstArg);
        }
        else
        {
          join_path_like(str, firstArg);
        }
      }

      template <typename PathLikeType, typename... Args>
      void join_impl(rsl::string& str, PathLikeType&& firstArg, Args&&... args)
      {
        join_impl(str, rsl::forward<PathLikeType>(firstArg));
        join_impl(str, rsl::forward<Args>(args)...);          // append the rest
      }
    }                                                         // namespace internal
    // returns the seperation char for paths
    char8 seperation_char();

    // Return the max length a file can have
    constexpr s32 max_path_length()
    {
      // MAX_PATH on Windows is 260 by default
      // You switch a flag to support longer paths (bugged in Windows 11 though)
      // https://answers.microsoft.com/en-us/windows/forum/all/long-file-path-issues-in-windows-11-pro-with/0c440721-e35a-4b58-9f5a-645656768f9d
      // but by default this is disabled
      return 256;
    }

    // returns an array of path names that aren't allowed to be used
    const rsl::vector<rsl::string_view>& invalid_path_names();
    // returns an array of invalid characters for directories
    const rsl::vector<char8>& invalid_path_chars();
    // Join multiple paths together
    template <typename... PathLikeTypes>
    rsl::string join(PathLikeTypes&&... paths)
    {
      rsl::string res;
      internal::join_impl(res, rsl::forward<PathLikeTypes>(paths)...);

      if(!res.empty())
      {
        res.pop_back(); // remove the last seperation char
      }
      return res;
    }

    // returns true if it's a valid path, returns false otherwise
    bool is_valid_path(rsl::string_view path);
    // returns true if it's a valid filename, returns false otherwise
    bool is_valid_filename(rsl::string_view filename);

    // removes leading and trailing quotes from a path
    rsl::string_view remove_quotes(rsl::string_view path);
    // Changes the extension of a path string_view
    // If extension argument is empty, the extension is removed
    // if the path doesn't have an extension, the extension specified gets appended
    rsl::string change_extension(rsl::string_view path, rsl::string_view extension);
    // Returns the directory path of the given path
    rsl::string_view dir_name(rsl::string_view path);
    // Returns the extension of the given path
    rsl::string_view extension(rsl::string_view path);
    // Returns the filename of the given path
    rsl::string_view filename(rsl::string_view path);
    // Returns the filename of the given path without its extension
    rsl::string_view stem(rsl::string_view path);
    // Returns the absolute path for the given path
    rsl::string abs_path(rsl::string_view path);
    // Retruns the root directory path of the given path
    rsl::string_view path_root(rsl::string_view path);
    // Returns a random directory, but doesn't create it
    rsl::string random_dir();
    // Returns a random filename, but doesn't create it
    rsl::string random_filename();
    // Returns the longest common sub-path of each pathname in the sequence
    rsl::string_view common_path(const rsl::vector<rsl::string_view>& paths);
    // Normalizes the path, removing redundant dots for current and parent directories
    // Converts forward slashes to backward slashes
    rsl::string norm_path(rsl::string_view path);
    // Returns a relative path to path, starting from the current working directory
    rsl::string rel_path(rsl::string_view path);
    // Returns a relative path to path, starting from the start directory
    rsl::string rel_path(rsl::string_view path, rsl::string_view start);
    // Returns if the given path has an extension
    bool has_extension(rsl::string_view path);
    // Returns if the given path is a relative path
    bool is_relative(rsl::string_view path);
    // Returns true if 2 paths point to the same file or directory
    bool is_same(rsl::string_view path1, rsl::string_view path2);

    // returns how deep a path is
    // basically counts the number of slashes
    // it does this without converting it to an absolute path
		// a file directly under the root has a depth of 1, any subdirectory increments depth by 1
    // this makes it so that the root always has a depth of 0
    s32 depth(rsl::string_view path, rsl::string_view root = cwd());
    // returns how deep from the root path is
    // basically counts the number of slashes
    // a file directly under the root has a depth of 1, any subdirectory increments depth by 1
    // this makes it so that the root always has a depth of 0
    s32 abs_depth(rsl::string_view path);
    // Returns true if the path starts with a drive letter, false otherwise
    bool has_drive(rsl::string_view path);
    // Returns true if the path given is the drive
    bool is_drive(rsl::string_view path);
    // Returns true if the path is pointing to the root
    bool is_root(rsl::string_view path);
    // Returns true if both paths are on the same mount
    bool has_same_root(rsl::string_view lhs, rsl::string_view rhs);
  } // namespace path
} // namespace rex

#ifdef REX_PLATFORM_WINDOWS
  #include "rex_engine/platform/win/filesystem/win_path.h"
#endif