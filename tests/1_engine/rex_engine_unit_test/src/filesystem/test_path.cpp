#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_std/chrono.h"
#include "rex_std/thread.h"
#include "rex_std/iostream.h"

#include <Windows.h>
#include <ShlObj.h>

TEST_CASE("Path Joining")
{
  REX_CHECK(rex::path::join("D:", "foo", "bar") == "D:/foo/bar");
  REX_CHECK(rex::path::join("foo", "bar") == "foo/bar");
  REX_CHECK(rex::path::join("foo", "bar.txt") == "foo/bar.txt");
}

TEST_CASE("Change Extension")
{
  REX_CHECK(rex::path::change_extension("foo.txt", ".bar") == "foo.bar");
  REX_CHECK(rex::path::change_extension("foo.txt", "") == "foo");
  REX_CHECK(rex::path::change_extension("foo", ".bar") == "foo.bar");
  REX_CHECK(rex::path::change_extension("foo", "") == "foo");

  REX_CHECK(rex::path::change_extension("bar/foo.txt", ".bar") == "bar/foo.bar");
  REX_CHECK(rex::path::change_extension("bar/foo.txt", "") == "bar/foo");
  REX_CHECK(rex::path::change_extension("bar/foo", ".bar") == "bar/foo.bar");
  REX_CHECK(rex::path::change_extension("bar/foo", "") == "bar/foo");
}

TEST_CASE("Dir Name")
{
  REX_CHECK(rex::path::dir_name("foo.txt") == "");
  REX_CHECK(rex::path::dir_name("bar/foo.txt") == "bar");
}

TEST_CASE("Get Extension")
{
  REX_CHECK(rex::path::extension("foo.txt") == ".txt");
  REX_CHECK(rex::path::extension("foo.txt.bar") == ".txt.bar");
  REX_CHECK(rex::path::extension("foo") == "");
  
  REX_CHECK(rex::path::extension("bar/foo.txt") == ".txt");
  REX_CHECK(rex::path::extension("bar/foo.txt.bar") == ".txt.bar");
  REX_CHECK(rex::path::extension("bar/foo") == "");
}

TEST_CASE("Get Stem")
{
  REX_CHECK(rex::path::stem("foo.txt") == "foo");
  REX_CHECK(rex::path::stem("foo.txt.bar") == "foo");
  REX_CHECK(rex::path::stem("foo") == "foo");

  REX_CHECK(rex::path::stem("bar/foo.txt") == "foo");
  REX_CHECK(rex::path::stem("bar/foo.txt.bar") == "foo");
  REX_CHECK(rex::path::stem("bar/foo") == "foo");
}

TEST_CASE("Absolute Path")
{
  rsl::string cwd = rex::path::cwd();

  REX_CHECK(rex::path::abs_path("foo.txt") == rex::path::join(cwd, "foo.txt"));
  REX_CHECK(rex::path::abs_path("foo.txt.bar") == rex::path::join(cwd, "foo.txt.bar"));
  REX_CHECK(rex::path::abs_path("foo") == rex::path::join(cwd, "foo"));

  REX_CHECK(rex::path::abs_path("bar/foo.txt") == rex::path::join(cwd, "bar", "foo.txt"));
  REX_CHECK(rex::path::abs_path("bar/foo.txt.bar") == rex::path::join(cwd, "bar", "foo.txt.bar"));
  REX_CHECK(rex::path::abs_path("bar/foo") == rex::path::join(cwd, "bar", "foo"));

  REX_CHECK(rex::path::abs_path("c:/bar/foo.txt") == "c:/bar/foo.txt");
  REX_CHECK(rex::path::abs_path("c:/bar/foo.txt.bar") == "c:/bar/foo.txt.bar");
  REX_CHECK(rex::path::abs_path("c:/bar/foo") == "c:/bar/foo");
}

TEST_CASE("Path Root")
{
  REX_CHECK(rex::path::path_root("c:/bar/foo.txt") == "c:");
  REX_CHECK(rex::path::path_root("bar/foo") == "");
  REX_CHECK(rex::path::path_root("./bar/foo") == ""); // windows only, so root is empty
  REX_CHECK(rex::path::path_root("/bar/foo") == ""); // windows only, so root is empty
  REX_CHECK(rex::path::path_root("~/bar/foo") == ""); // windows only, so root is empty
}

TEST_CASE("Common Path")
{
  {
    rsl::vector<rsl::string_view> paths =
    {
      "c:/bar/foo.txt",
      "c:/bar/text.txt"
    };

    REX_CHECK(rex::path::common_path(paths) == "c:/bar/");
  }

  {
    rsl::vector<rsl::string_view> paths =
    {
      "c:/foo/foo.txt",
      "c:/bar/text.txt"
    };

    REX_CHECK(rex::path::common_path(paths) == "c:/");
  }

  {
    rsl::vector<rsl::string_view> paths =
    {
      "c:/bar/foo.txt",
      "d:/bar/text.txt"
    };

    REX_CHECK(rex::path::common_path(paths) == "");
  }
}

TEST_CASE("Random Dir")
{
  for (int i = 0; i < 1000; ++i)
  {
    REX_CHECK(!rex::file::exists(rex::path::random_dir()));
  }
}

TEST_CASE("Random Filename")
{
  for (int i = 0; i < 1000; ++i)
  {
    REX_CHECK(!rex::file::exists(rex::path::random_filename()));
  }
}

TEST_CASE("Real Path")
{
  // Make sure that the working directory is set correctly here
  rsl::string real_path = rex::path::real_path("shortcut_to_foo.txt.lnk");
  rsl::to_lower(real_path.cbegin(), real_path.begin(), real_path.size());
  REX_CHECK(real_path.ends_with(rex::path::join("rexengineunittest", "foo.txt")));
}

TEST_CASE("Normalise Path")
{
  REX_CHECK(rex::path::norm_path("/foo/./bar") == rex::path::join("foo", "bar"));
  REX_CHECK(rex::path::norm_path("/path/to/dir") == rex::path::join("path","to","dir"));
  REX_CHECK(rex::path::norm_path("path/to/dir") == rex::path::join("path","to","dir"));
  REX_CHECK(rex::path::norm_path("/") == "");

  REX_CHECK(rex::path::norm_path("/path/./to/./dir") == rex::path::join("path","to","dir"));
  REX_CHECK(rex::path::norm_path("/path/to/../dir") == rex::path::join("path","dir"));
  REX_CHECK(rex::path::norm_path("/path/to/../../dir") == "dir");

  REX_CHECK(rex::path::norm_path("/path/to/dir\\file") == rex::path::join("path","to","dir","file"));
  REX_CHECK(rex::path::norm_path("\\path\\to\\dir\\file") == rex::path::join("path", "to", "dir", "file"));
  REX_CHECK(rex::path::norm_path("path/to/dir/file\\") == rex::path::join("path", "to", "dir", "file"));

  REX_CHECK(rex::path::norm_path("//path/to//dir") == rex::path::join("path","to","dir"));
  REX_CHECK(rex::path::norm_path("/\\path\\to/dir") == rex::path::join("path","to","dir"));
  REX_CHECK(rex::path::norm_path("/path/to/dir\\") == rex::path::join("path","to","dir"));

  REX_CHECK(rex::path::norm_path("/path//to//dir") == rex::path::join("path","to","dir"));
  REX_CHECK(rex::path::norm_path("/path/to//dir") == rex::path::join("path","to","dir"));
  REX_CHECK(rex::path::norm_path("/path/to/dir//") == rex::path::join("path","to","dir"));

  REX_CHECK(rex::path::norm_path("/path//to//dir/") == rex::path::join("path","to","dir"));
  REX_CHECK(rex::path::norm_path("../..") == rex::path::join("..",".."));
  REX_CHECK(rex::path::norm_path("path/../..") == "..");

  REX_CHECK(rex::path::norm_path("C:\\path\\to\\dir") == rex::path::join("C:","path","to","dir"));
  REX_CHECK(rex::path::norm_path("C:\\path\\to\\..\\dir") == rex::path::join("C:","path","dir"));
}

TEST_CASE("Relative Path")
{
  REX_CHECK(rex::path::rel_path("/path/to/dir", "/path") == rex::path::join("to", "dir"));
  REX_CHECK(rex::path::rel_path("path/to/dir", "path") == rex::path::join("to", "dir"));
  REX_CHECK(rex::path::rel_path("/", "/") == rex::path::join(""));

  REX_CHECK(rex::path::rel_path("/path/./to/./dir", "/path") == rex::path::join("to", "dir"));
  REX_CHECK(rex::path::rel_path("/path/to/../dir", "/path") == rex::path::join("dir"));
  REX_CHECK(rex::path::rel_path("/path/to/../../dir", "/path") == rex::path::join("..", "dir"));

  REX_CHECK(rex::path::rel_path("/path/to/dir\\file", "/path") == rex::path::join("to", "dir", "file"));
  REX_CHECK(rex::path::rel_path("\\path\\to\\dir\\file", "/path") == rex::path::join("to", "dir", "file"));
  REX_CHECK(rex::path::rel_path("path/to/dir/file\\", "path") == rex::path::join("to", "dir", "file"));

  REX_CHECK(rex::path::rel_path("//path/to//dir", "/") == rex::path::join("path", "to", "dir"));
  REX_CHECK(rex::path::rel_path("/\\path\\to/dir", "/") == rex::path::join("path", "to", "dir"));
  REX_CHECK(rex::path::rel_path("/path/to/dir\\", "/") == rex::path::join("path", "to", "dir"));

  REX_CHECK(rex::path::rel_path("/path//to//dir", "/") == rex::path::join("path", "to", "dir"));
  REX_CHECK(rex::path::rel_path("/path/to//dir", "/") == rex::path::join("path", "to", "dir"));
  REX_CHECK(rex::path::rel_path("/path/to/dir//", "/") == rex::path::join("path", "to", "dir"));

  REX_CHECK(rex::path::rel_path("/path//to//dir/", "/") == rex::path::join("path", "to", "dir"));
  REX_CHECK(rex::path::rel_path("/", "path/to/dir") == rex::path::join("..", "..", ".."));

  REX_CHECK(rex::path::rel_path("C:\\path\\to\\dir", "C:\\path") == rex::path::join("to", "dir"));
  REX_CHECK(rex::path::rel_path("C:\\path\\to\\..\\dir", "C:\\path") == rex::path::join("dir"));
}

TEST_CASE("Is Under Dir")
{
  REX_CHECK(rex::path::is_under_dir("/path/to/dir", "/path") == true);
  REX_CHECK(rex::path::is_under_dir("path/to/dir", "path") == true);
  REX_CHECK(rex::path::is_under_dir("/", "/") == true);

  REX_CHECK(rex::path::is_under_dir("/path/./to/./dir", "/path") == true);
  REX_CHECK(rex::path::is_under_dir("/path/to/../dir", "/path") == true);
  REX_CHECK(rex::path::is_under_dir("/path/to/../../dir", "/path") == false);

  REX_CHECK(rex::path::is_under_dir("/path/to/dir\\file", "/path") == true);
  REX_CHECK(rex::path::is_under_dir("\\path\\to\\dir\\file", "/path") == true);
  REX_CHECK(rex::path::is_under_dir("path/to/dir/file\\", "path") == true);

  REX_CHECK(rex::path::is_under_dir("//path/to//dir", "/") == true);
  REX_CHECK(rex::path::is_under_dir("/\\path\\to/dir", "/") == true);
  REX_CHECK(rex::path::is_under_dir("/path/to/dir\\", "/") == true);

  REX_CHECK(rex::path::is_under_dir("/path//to//dir", "/") == true);
  REX_CHECK(rex::path::is_under_dir("/path/to//dir", "/") == true);
  REX_CHECK(rex::path::is_under_dir("/path/to/dir//", "/") == true);

  REX_CHECK(rex::path::is_under_dir("/path//to//dir/", "/") == true);
  REX_CHECK(rex::path::is_under_dir("/", "path/to/dir") == false);

  REX_CHECK(rex::path::is_under_dir("C:\\path\\to\\dir", "C:\\path") == true);
  REX_CHECK(rex::path::is_under_dir("C:\\path\\to\\..\\dir", "C:\\path") == true);
}

TEST_CASE("Has Extension")
{
  REX_CHECK(rex::path::has_extension("file.txt") == true);
  REX_CHECK(rex::path::has_extension("document.pdf") == true);
  REX_CHECK(rex::path::has_extension("/path/to/file.txt") == true);
  REX_CHECK(rex::path::has_extension("/path/to/document.pdf") == true);

  REX_CHECK(rex::path::has_extension("file") == false);
  REX_CHECK(rex::path::has_extension("/path/to/file") == false);
  REX_CHECK(rex::path::has_extension("no_extension.") == false);
  REX_CHECK(rex::path::has_extension("/path/to/no_extension.") == false);

  REX_CHECK(rex::path::has_extension("") == false);  // Empty string
  REX_CHECK(rex::path::has_extension(".") == false);  // Only a dot, not a valid extension
  REX_CHECK(rex::path::has_extension("..") == false); // Double dots, not a valid extension
}

TEST_CASE("Is Absolute")
{
    REX_CHECK(rex::path::is_absolute("/absolute/path") == true);
    REX_CHECK(rex::path::is_absolute("/another/absolute/path/file.txt") == true);
    REX_CHECK(rex::path::is_absolute("C:\\Windows\\System32\\file.dll") == true);  // Windows path

    REX_CHECK(rex::path::is_absolute("relative/path") == false);
    REX_CHECK(rex::path::is_absolute("./relative/path") == false);
    REX_CHECK(rex::path::is_absolute("../relative/path/file.txt") == false);

    REX_CHECK(rex::path::is_absolute("") == false);  // Empty string
    REX_CHECK(rex::path::is_absolute(".") == false);  // Current directory
    REX_CHECK(rex::path::is_absolute("..") == false); // Parent directory
}

TEST_CASE("Is Relative")
{
    REX_CHECK(rex::path::is_relative("relative/path") == true);
    REX_CHECK(rex::path::is_relative("./relative/path") == true);
    REX_CHECK(rex::path::is_relative("../relative/path/file.txt") == true);

    REX_CHECK(rex::path::is_relative("/absolute/path") == false);
    REX_CHECK(rex::path::is_relative("/another/absolute/path/file.txt") == false);
    REX_CHECK(rex::path::is_relative("C:\\Windows\\System32\\file.dll") == false);  // Windows path

    REX_CHECK(rex::path::is_relative("") == false);  // Empty string
    REX_CHECK(rex::path::is_relative(".") == false);  // Current directory
    REX_CHECK(rex::path::is_relative("..") == false); // Parent directory
}

TEST_CASE("Is Junction")
{
  REX_CHECK(rex::path::is_junction("original_folder") == false);
  REX_CHECK(rex::path::is_junction("original_file.txt") == false);
  //REX_CHECK(rex::path::is_junction("junction_folder") == true); // not testable as junction can't be put on version control
}

TEST_CASE("Is Link")
{
  REX_CHECK(rex::path::is_link("original_folder") == false);
  REX_CHECK(rex::path::is_link("original_file.txt") == false);
  REX_CHECK(rex::path::is_link("junction_folder") == false);
}

TEST_CASE("Is Same File")
{
  REX_CHECK(rex::path::same_path("same_file.txt", "same_file.txt") == true);
  REX_CHECK(rex::path::same_path("sub_folder_1/../same_file.txt", "same_file.txt") == true);
  REX_CHECK(rex::path::same_path("same_file.txt", rex::path::random_filename()) == false);
  REX_CHECK(rex::path::same_path("same_file.txt", rex::path::random_dir()) == false);


  REX_CHECK(rex::path::same_path("sub_folder_1", "sub_folder_1") == true);
  REX_CHECK(rex::path::same_path("sub_folder_1/../sub_folder_1", "sub_folder_1") == true);
  REX_CHECK(rex::path::same_path("same_file.txt", rex::path::random_filename()) == false);
  REX_CHECK(rex::path::same_path("same_file.txt", rex::path::random_dir()) == false);

  REX_CHECK(rex::path::same_path(rex::path::random_filename(), rex::path::random_filename()) == false);
  REX_CHECK(rex::path::same_path(rex::path::random_filename(), rex::path::random_dir()) == false);
  REX_CHECK(rex::path::same_path(rex::path::random_dir(), rex::path::random_dir()) == false);
}

TEST_CASE("Split")
{
    auto result = rex::path::split("/path/to/file.txt");
    REX_CHECK(result.head == "/path/to");
    REX_CHECK(result.tail == "file.txt");

    result = rex::path::split("C:\\Windows\\System32\\file.dll");
    REX_CHECK(result.head == "C:\\Windows\\System32");
    REX_CHECK(result.tail == "file.dll");

    result = rex::path::split("./relative/path");
    REX_CHECK(result.head == "./relative");
    REX_CHECK(result.tail == "path");

    result = rex::path::split("");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "");

    result = rex::path::split(".");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == ".");

    result = rex::path::split("..");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "..");
}

TEST_CASE("Split Origin")
{
    auto result = rex::path::split_origin("C:\\Windows\\System32\\file.dll");
    REX_CHECK(result.head == "C:");
    REX_CHECK(result.tail == "Windows\\System32\\file.dll");

    result = rex::path::split_origin("\\Windows\\System32\\file.dll");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "\\Windows\\System32\\file.dll");

    result = rex::path::split_origin("/path/to/file.txt");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "/path/to/file.txt");

    result = rex::path::split_origin("");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "");

    result = rex::path::split_origin("./relative/path");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "./relative/path");
}

TEST_CASE("Split Extension")
{
    auto result = rex::path::split_ext("/path/to/file.txt");
    REX_CHECK(result.head == "/path/to/file");
    REX_CHECK(result.tail == ".txt");

    result = rex::path::split_ext("C:\\Windows\\System32\\file.dll");
    REX_CHECK(result.head == "C:\\Windows\\System32\\file");
    REX_CHECK(result.tail == ".dll");

    result = rex::path::split_ext("./relative/path/file.txt");
    REX_CHECK(result.head == "./relative/path/file");
    REX_CHECK(result.tail == ".txt");

    result = rex::path::split_ext("/path/to/file");
    REX_CHECK(result.head == "/path/to/file");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("C:\\Windows\\System32\\file");
    REX_CHECK(result.head == "C:\\Windows\\System32\\file");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("./relative/path/file");
    REX_CHECK(result.head == "./relative/path/file");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext(".");
    REX_CHECK(result.head == ".");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("..");
    REX_CHECK(result.head == "..");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("./.");
    REX_CHECK(result.head == "./.");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("../..");
    REX_CHECK(result.head == "../..");
    REX_CHECK(result.tail == "");
}

TEST_CASE("Split Root")
{
    auto result = rex::path::split_root("C:\\Windows\\System32\\file.dll");
    REX_CHECK(result.drive == "C:");
    REX_CHECK(result.root == "\\");
    REX_CHECK(result.tail == "Windows\\System32\\file.dll");

    result = rex::path::split_root("\\Windows\\System32\\file.dll");
    REX_CHECK(result.drive == "");
    REX_CHECK(result.root == "\\");
    REX_CHECK(result.tail == "Windows\\System32\\file.dll");

    result = rex::path::split_root("/path/to/file.txt");
    REX_CHECK(result.drive == "");
    REX_CHECK(result.root == "/");
    REX_CHECK(result.tail == "path/to/file.txt");

    result = rex::path::split_root("relative/path/file.txt");
    REX_CHECK(result.drive == "");
    REX_CHECK(result.root == "");
    REX_CHECK(result.tail == "relative/path/file.txt");

    result = rex::path::split_root("");
    REX_CHECK(result.drive == "");
    REX_CHECK(result.root == "");
    REX_CHECK(result.tail == "");

    result = rex::path::split_root("./relative/path");
    REX_CHECK(result.drive == "");
    REX_CHECK(result.root == "");
    REX_CHECK(result.tail == "./relative/path");
}