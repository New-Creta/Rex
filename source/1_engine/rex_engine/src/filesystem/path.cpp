#include "rex_engine/filesystem/path.h"

#include "rex_engine/engine/numeric.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/filesystem/file.h"
#include "rex_std/algorithm.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/string.h"
#include "rex_std/ctype.h"
#include "rex_std/format.h"
// The current implementation of this namespace is Windows only

namespace rex
{
  namespace path
  {
    constexpr char8 g_seperation_char = '/';

    namespace internal
    {
      // concat the arg to the string in filepath format
      void join_string_view(rsl::string& str, rsl::string_view arg)
      {
        if(arg.empty())
        {
          return;
        }

        str += arg;

        if(!str.ends_with(seperation_char()))
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
        auto pos      = path.find_last_of("/\\");
        auto filename = pos != path.npos() // NOLINT(readability-static-accessed-through-instance)
                            ? path.substr(pos + 1)
                            : path;

        // current dir and parent dir "filenames" don't have extensions
        if(filename == "." || filename == "..")
        {
          return path.npos(); // NOLINT(readability-static-accessed-through-instance)
        }

        auto ext_start = filename.find_first_of('.');

        // make sure we convert the pos in the filename back to the pos
        // of the total input path
        return ext_start != filename.npos() // NOLINT(readability-static-accessed-through-instance)
                   ? static_cast<card32>(&filename[ext_start] - path.data())
                   : path.npos();           // NOLINT(readability-static-accessed-through-instance)
      }

      // Fills a string with a number of random characters
      // This is useful for creating random filenames and directories
      void fill_with_random_chars(rsl::string& str, card32 numCharsToFill)
      {
        rsl::small_stack_string chars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
        for(card32 i = 0; i < numCharsToFill; ++i)
        {
          const card32 random_idx = std::rand() % chars.length(); // NOLINT(cert-msc50-cpp, concurrency-mt-unsafe)
          str += chars[random_idx];
        }
      }

    } // namespace internal

    // returns the seperation char for paths
    char8 seperation_char()
    {
      return g_seperation_char;
    }

    // returns an array of invalid characters for filenames
    const rsl::vector<char8>& invalid_path_chars()
    {
      static rsl::vector<char8> res = {
          '"',
          '<',
          '>',
          '|',
          '*',
          '?',
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
    // returns an array of path names that aren't allowed to be used
    const rsl::vector<rsl::string_view>& invalid_path_names()
    {
      static rsl::vector<rsl::string_view> res = {
        // Filenames on windows that are reserved for device names
        "con",
        "aux",
        "nul",
        "com1",
        "lpt1"
      };

      return res;
    }

    // returns true if it's a valid path, returns false otherwise
    bool is_valid_path(rsl::string_view path)
    {
      if (path.length() > max_path_length())
      {
        return false;
      }

      if (path.length() == 0)
      {
        return true;
      }

      SplitResult split_origin_res = split_origin(path);
      const rsl::vector<rsl::string_view> splitted = rsl::split(split_origin_res.tail, "/\\");
      
      for (rsl::string_view subpath : splitted)
      {
        if (!is_valid_filename(subpath))
        {
          return false;
        }
      }

      return true;

      //const rsl::string_view invalid_chars(invalid_path_chars().data(), invalid_path_chars().size());
      //return path.find_first_of(invalid_chars) == path.npos(); // NOLINT(readability-static-accessed-through-instance)
    }
    // returns true if it's a valid filename, returns false otherwise
    bool is_valid_filename(rsl::string_view filename)
    {
      if (filename.length() == 0)
      {
        return false;
      }

      // files are not allowed to have slashes or colons
      if (filename.find_first_of("/\\:") != filename.npos())
      {
        return false;
      }

      rsl::string filename_lower(filename);
      rsl::to_lower(filename_lower.cbegin(), filename_lower.begin(), filename_lower.length());
      if (rsl::find(invalid_path_names().cbegin(), invalid_path_names().cend(), filename_lower) != invalid_path_names().cend())
      {
        return false;
      }

      const rsl::string_view invalid_chars(invalid_path_chars().data(), invalid_path_chars().size());
      return filename.find_first_of(invalid_chars) == filename.npos(); // NOLINT(readability-static-accessed-through-instance)
    }

    // removes leading and trailing quotes from a path
    rsl::string_view remove_quotes(rsl::string_view path)
    {
      return rex::remove_quotes(path);
    }
    // Changes the extension of a path string_view
    // If extension argument is empty, the extension is removed
    // if the path doesn't have an extension, the extension specified gets appended
    rsl::string change_extension(rsl::string_view path, rsl::string_view extension)
    {
      const SplitResult split_res = split_ext(path);

      // use the extension split to store the path without the extension
      rsl::string res(split_res.head);

      // Add a dot if the provided one doesn't have one
      if(!extension.empty() && !extension.starts_with('.'))
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
      const SplitResult split_res = split(path);
      return split_res.head;
    }
    // Returns the extension of the given path
    rsl::string_view extension(rsl::string_view path)
    {
      const SplitResult split_res = split_ext(path);
      return split_res.tail;
    }
    // Returns the filename of the given path
    rsl::string_view filename(rsl::string_view path)
    {
      const SplitResult split_res = split(path);
      return split_res.tail;
    }
    // Returns the filename of the given path without its extension
    rsl::string_view stem(rsl::string_view path)
    {
      // get the filename of the path
      const rsl::string_view file_name = filename(path);

      // get the position of the extension, if there is one
      const card32 extension_pos = internal::extension_start(file_name);
      const card32 count         = extension_pos != -1 ? extension_pos : file_name.length();

      // return the substring of the filename, without the extension
      return file_name.substr(0, count);
    }
    // Returns the absolute path for the given path
    rsl::string abs_path(rsl::string_view path)
    {
      // If the path is already absolute, just return it
      if(is_absolute(path))
      {
        rsl::string res(path);
        res.replace("\\", "/");
        if (abs_needs_drive() && !has_drive(res))
        {
          rsl::string cwd = path::cwd();
          res.insert(0, cwd.substr(0, 2)); // This prepends the drive letter and colon
        }
        return res;
      }

      // Get the current working directory and prepend it to the path
      rsl::string current_dir = path::cwd();
      rsl::string res         = path::join(current_dir, path);
      return res.replace("\\", "/");
    }
    // Returns the root directory path of the given path
    rsl::string_view path_root(rsl::string_view path)
    {
      // if the path is an absolute path, return the path root
      if(is_absolute(path))
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
      const card32 num_dirname_chars = 8;
      rsl::string result;

      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        // clear the result first before adding to it
        result.clear();

        // create a stem of 8 random characters
        internal::fill_with_random_chars(result, num_dirname_chars);

      } while(directory::exists(result));

      return result;
    }
    // Returns a random filename, but doesn't create it
    rsl::string random_filename()
    {
      const card32 num_stem_chars = 8;
      const card32 num_ext_chars  = 3;
      rsl::string result;

      do // NOLINT(cppcoreguidelines-avoid-do-while)
      {
        // clear the result first before adding to it
        result.clear();

        // create a stem of 8 random characters
        internal::fill_with_random_chars(result, num_stem_chars);
        result += '.';

        // create an extension of 3 random characters
        internal::fill_with_random_chars(result, num_ext_chars);

      } while(file::exists(result));

      return result;
    }

    // Returns the longest common sub-path of each pathname in the sequence
    rsl::string_view common_path(const rsl::vector<rsl::string_view>& paths)
    {
      // if no paths are given, just return an empty path
      if(paths.empty())
      {
        return "";
      }

      // split the first path into different path components
      const rsl::vector<rsl::string_view> splitted = rsl::split(paths.front(), "/\\");
      auto furthest_path_component_it              = splitted.cbegin();

      // iterate over the other paths
      for(card32 i = 1; i < paths.size(); ++i)
      {
        const rsl::string_view path = paths[i];

        // split the path into different path components
        const rsl::vector<rsl::string_view> splitted_path = rsl::split(path, "/\\");

        // find the first mismatch with the first path in the list
        auto res = rsl::mismatch(splitted.cbegin(), splitted.cend(), splitted_path.cbegin(), splitted_path.cend());

        // if None are equal, return an empty path
        if(res.lhs_it == splitted.cbegin())
        {
          return "";
        }

        // otherwise store the max iterator where the mismatch occurred on a previous run
        furthest_path_component_it = (rsl::max)(furthest_path_component_it, res.lhs_it);
      }

      // store the index of the path component to figure out where the common path ends
      const card32 path_component_idx = rsl::distance(splitted.cbegin(), furthest_path_component_it);

      // count the path components, so we know where out substring should end
      const rsl::string_view first_path = paths.front();
      card32 pos                        = 0;
      for(card32 i = 0; i < path_component_idx; ++i)
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
      const rsl::vector<rsl::string_view> splitted_path = rsl::split(path, "/\\");
      rsl::vector<rsl::string_view> norm_splitted(rsl::Capacity(splitted_path.size()));

      // loop over each path component in the given path
      for(const rsl::string_view path_comp: splitted_path)
      {
        // if the "current dir" token is found, we skip it
        if(path_comp == ".")
        {
          continue;
        }

        // if the parent dir is found, we remove the last added element
        // unless that's the parent dir token.
        // if the parent dir is found, it means there are no known parents anymore
        // then we add the parent dir to the normalized path components
        if(path_comp == "..")
        {
          if(!norm_splitted.empty() && norm_splitted.back() != "..")
          {
            norm_splitted.pop_back();
            continue;
          }
        }

        // in any other case, we add the path component to the list
        norm_splitted.push_back(path_comp);
      }

      // join everything back together and return the result
      rsl::string res;

      // For linux systems, an absolute path starts with a slash, and we need to add it back
      if (path.starts_with("/") || path.starts_with("\\"))
      {
        res += g_seperation_char;
      }

      res += rsl::join(norm_splitted, rsl::string_view(&g_seperation_char, 1)).as_string();

      if (is_drive(res))
      {
        res += g_seperation_char;
      }

      return res;
    }
    // Returns a relative path to path, starting from the current working directory
    rsl::string rel_path(rsl::string_view path)
    {
      return rel_path(path, cwd());
    }
    // Returns a relative path to path, starting from the start directory
    rsl::string rel_path(rsl::string_view path, rsl::string_view root)
    {
      // Return a path that can be appended to root and you'd get the path that "path" points to

      // Get rid of all unnecessary path tokens, making parsing of the path easier
      rsl::string norm_path  = path::norm_path(path);
      rsl::string norm_root = path::norm_path(root);

      // If both paths are empty, just return an empty path
      if(norm_path.empty() && norm_root.empty())
      {
        return rsl::string("");
      }

      // Convert both path to their absolute paths, so its even easier to parse
			rsl::string abs_norm_path = abs_path(norm_path);
			rsl::string abs_norm_root = abs_path(norm_root);

      // Make them lower case so we ignore any casing during string comparison
      rsl::to_lower(abs_norm_path.cbegin(), abs_norm_path.begin(), abs_norm_path.size());
      rsl::to_lower(abs_norm_root.cbegin(), abs_norm_root.begin(), abs_norm_root.size());

      // If both paths are equal, return an empty string
      if (is_same(abs_norm_path, abs_norm_root))
      {
        return rsl::string("");
      }

      // If there is a mismatch however, we need to find where this happens and construct a path from the root to this path
      // Afterwards we can just append the remaining path
      rsl::vector<rsl::string_view> paths{ abs_norm_path, abs_norm_root };
      rsl::string_view common = common_path(paths);

      rsl::string_view diff_in_path = abs_norm_path.substr(common.length());
      rsl::string_view diff_in_root = abs_norm_root.substr(common.length());

      // Find the first mismatch between the paths, if there's none, the paths are equal
      const rsl::vector<rsl::string_view> splitted_path = rsl::split(diff_in_path, rsl::string_view(&g_seperation_char, 1));
      const rsl::vector<rsl::string_view> splitted_start = rsl::split(diff_in_root, rsl::string_view(&g_seperation_char, 1));

      auto mismatch_res = rsl::mismatch(splitted_path.cbegin(), splitted_path.cend(), splitted_start.cbegin(), splitted_start.cend());

      s32 num_parent_dir_tokens = splitted_start.size();
      rsl::string result;
			for (card32 i = 0; i < num_parent_dir_tokens; ++i)
			{
				result = path::join(result, "..");
			}
      result = path::join(result, diff_in_path);
      return result;
    }

    // Returns if the given path has an extension
    bool has_extension(rsl::string_view path)
    {
      if(path == ".")
      {
        return false;
      }

      if(path == "..")
      {
        return false;
      }

      auto start = internal::extension_start(path);
      return start != rsl::string_view::npos() && start != path.size() - 1;
    }

    // Returns if a file is under a certain directory
    bool is_under_dir(rsl::string_view path, rsl::string_view dir)
    {
      rsl::string relative_path = rel_path(path, dir);
      return !relative_path.starts_with("..");
    }

    // Returns if the given path is a relative path
    bool is_relative(rsl::string_view path)
    {
      if(path.empty())
      {
        return false;
      }

      if(path == ".")
      {
        return false;
      }

      if(path == "..")
      {
        return false;
      }

      return !is_absolute(path);
    }

    // Returns true if 2 paths point to the same file
    bool is_same(rsl::string_view path1, rsl::string_view path2)
    {
      // simply convert the files into their actual files on disk
      // then do a string wise comparison
      rsl::string real_path1 = real_path(path1);
      rsl::string real_path2 = real_path(path2);

      rsl::to_lower(real_path1.cbegin(), real_path1.begin(), real_path1.size());
      rsl::to_lower(real_path2.cbegin(), real_path2.begin(), real_path2.size());
      
      return real_path1 == real_path2;
    }
    // Splits the path into a head and a tail
    // the tail is the last pathname component
    // the head is everything leading up to that
    SplitResult split(rsl::string_view path)
    {
      SplitResult res {};

      // get the last slash position
      const card32 pos = path.find_last_of("/\\");

      // fill in the values
      if(pos != path.npos()) // NOLINT(readability-static-accessed-through-instance)
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
      SplitResult res {};

      // get the extension start position
      const card32 ext_start = internal::extension_start(path);

      // fill in the values
      if(ext_start != path.npos()) // NOLINT(readability-static-accessed-through-instance)
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
    s32 depth(rsl::string_view path, rsl::string_view root)
    {
      if (path.empty())
      {
        return 0;
      }

      if (!is_under_dir(path, root))
      {
        return 0;
      }

      s32 root_depth = abs_depth(root);
      s32 path_depth = abs_depth(path);

      return path_depth - root_depth;
    }
    s32 abs_depth(rsl::string_view path)
    {
      rsl::string fullpath = abs_path(norm_path(path));

      if (is_root(fullpath))
      {
        return 0;
      }

      s32 slash_count = rsl::count(fullpath.cbegin(), fullpath.cend(), '/');
      s32 backwards_slash_count = rsl::count(fullpath.cbegin(), fullpath.cend(), '\\');

      return (slash_count + backwards_slash_count);
    }
    bool has_drive(rsl::string_view path)
    {
      if (path.length() < 3)
      {
        return false;
      }

      return
        rsl::is_alpha(path[0]) &&
        path[1] == ':' &&
        (path[2] == '/' || path[2] == '\\');
    }
    bool is_drive(rsl::string_view path)
    {
      if (path.length() != 2)
      {
        return false;
      }

      return rsl::is_alpha(path[0]) &&
        path[1] == ':';
    }
    // Returns true fi the path is pointing to the root
    bool is_root(rsl::string_view path)
    {
      // Only slashes
      if (path.length() > 0 && path.find_first_not_of("/\\") == path.npos())
      {
        return true;
      }

      // Drive root
      return path.length() == 3 && has_drive(path);
    }
  } // namespace path
} // namespace rex