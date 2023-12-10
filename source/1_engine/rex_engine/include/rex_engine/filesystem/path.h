#pragma once

#include "rex_std/array.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace path
  {
    namespace internal
    {
      template <typename PathLikeType, typename ... Args>
      void join_impl(rsl::string& str, PathLikeType&& arg)
      {
        str += arg;
       
        if (!str.ends_with(seperation_char()))
        {
          str += seperation_char();
        }
      }
      template <typename PathLikeType, typename ... Args>
      void join_impl(rsl::string& str, PathLikeType&& firstArg, Args&& ... args)
      {
        join_impl(str, rsl::forward<PathLikeType>(firstArg)); // append the first arg
        join_impl(str, rsl::forward<Args>(args)...); // append the rest
      }
    }
    // returns the seperation char for paths
    char8 seperation_char();

    // returns an array of invalid characters for filenames
    constexpr auto invalid_file_name_chars()
    {
      rsl::array res = {
        '"',
        '<',
        '>',
        '|',
        ':',
        '*',
        '?',
        '\\',
        '/',
        static_cast<char8>(0),
        static_cast<char8>(1),
        static_cast<char8>(2),
        static_cast<char8>(3),
        static_cast<char8>(4),
        static_cast<char8>(5),
        static_cast<char8>(6),
        static_cast<char8>(7),
        static_cast<char8>(8),
        static_cast<char8>(9),
        static_cast<char8>(10),
        static_cast<char8>(11),
        static_cast<char8>(12),
        static_cast<char8>(13),
        static_cast<char8>(14),
        static_cast<char8>(15),
        static_cast<char8>(16),
        static_cast<char8>(17),
        static_cast<char8>(18),
        static_cast<char8>(19),
        static_cast<char8>(20),
        static_cast<char8>(21),
        static_cast<char8>(22),
        static_cast<char8>(23),
        static_cast<char8>(24),
        static_cast<char8>(25),
        static_cast<char8>(26),
        static_cast<char8>(27),
        static_cast<char8>(28),
        static_cast<char8>(29),
        static_cast<char8>(30),
        static_cast<char8>(31),
      };

      return res;
    }
    // returns an array of invalid characters for directories
    constexpr auto invalid_path_chars()
    {
      rsl::array res = {
      '"',
      '<',
      '>',
      '|',
      static_cast<char8>(0),
      static_cast<char8>(1),
      static_cast<char8>(2),
      static_cast<char8>(3),
      static_cast<char8>(4),
      static_cast<char8>(5),
      static_cast<char8>(6),
      static_cast<char8>(7),
      static_cast<char8>(8),
      static_cast<char8>(9),
      static_cast<char8>(10),
      static_cast<char8>(11),
      static_cast<char8>(12),
      static_cast<char8>(13),
      static_cast<char8>(14),
      static_cast<char8>(15),
      static_cast<char8>(16),
      static_cast<char8>(17),
      static_cast<char8>(18),
      static_cast<char8>(19),
      static_cast<char8>(20),
      static_cast<char8>(21),
      static_cast<char8>(22),
      static_cast<char8>(23),
      static_cast<char8>(24),
      static_cast<char8>(25),
      static_cast<char8>(26),
      static_cast<char8>(27),
      static_cast<char8>(28),
      static_cast<char8>(29),
      static_cast<char8>(30),
      static_cast<char8>(31),
      };
      return res;
    }
    // Join multiple paths together
    template <typename ... PathLikeTypes>
    rsl::string join(PathLikeTypes&& ... paths)
    {
      rsl::string res;
      internal::join_impl(res, rsl::forward<PathLikeTypes>(paths)...);

      res.pop_back(); // remove the last seperation char
      return res;
    }

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
    // Returns the current working directory
    rsl::string cwd();
    // Returns a random directory, but doesn't create it
    rsl::string random_dir();
    // Returns a random filename, but doesn't create it
    rsl::string random_filename();
    // Returns the path of the current user's temp folder
    rsl::string temp_path();
    // Returns the longest common sub-path of each pathname in the sequence
    rsl::string_view common_path(const rsl::vector<rsl::string_view>& paths);
    // For symlinks, returns the path the link points to
    // Otherwise returns the input
    rsl::string real_path(rsl::string_view path);
    // Normalizes the path, removing redundant dots for current and parent directories
    // Converts forward slashes to backward slashes
    rsl::string norm_path(rsl::string_view path);
    // Returns a relative path to path, starting from the start directory
    rsl::string rel_path(rsl::string_view path, rsl::string_view start = cwd());
    // Returns the latest access time of the file or directory at the given path
    card64 get_access_time(rsl::string_view path);
    // Returns the modification time of the file or directory at the given path
    card64 get_modification_time(rsl::string_view path);
    // Returns the creation time of the file or directory at the given path
    card64 get_creation_time(rsl::string_view path);
    // Returns the creation time of the file or directory at the given path
    card64 get_file_size(rsl::string_view path);
    // Returns if the given path has an extension
    bool has_extension(rsl::string_view path);
    // Returns if the path exists
    bool exists(rsl::string_view path);
    // Returns if the given path is an absolute path
    bool is_absolute(rsl::string_view path);
    // Returns if the given path is a relative path
    bool is_relative(rsl::string_view path);
    // Returns true if the given path points to a file
    bool is_file(rsl::string_view path);
    // Returns true if the given path points to a directory
    bool is_dir(rsl::string_view path);
    // Returns true if the given path points to a junction
    bool is_junction(rsl::string_view path);
    // Returns true if the given path points to a symlink
    bool is_link(rsl::string_view path);
    // Returns true if 2 paths point to the same file
    bool same_file(rsl::string_view path1, rsl::string_view path2);
    // Returns true if 2 paths point to the same directory
    bool same_dir(rsl::string_view path);
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
    // the head is either the mount point or an empty string
    // the tail is everything else
    SplitResult split_drive(rsl::string_view path);
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
  }
}