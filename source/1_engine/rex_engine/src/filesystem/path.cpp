#include "rex_engine/filesystem/path.h"

#include "rex_engine/engine/numeric.h"
#include "rex_std/algorithm.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/ctype.h"
#include "rex_std/format.h"
#include "rex_std/bonus/string.h"

// The current implementation of this namespace is Windows only

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
        // we need to scan for the first dot 
        // after the last slash
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
    // Returns a relative path to path, starting from the current working directory
    rsl::string rel_path(rsl::string_view path)
    {
      return rel_path(path, cwd());
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
  } // namespace path
} // namespace rex