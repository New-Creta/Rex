#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/path.h"
#include "rex_std/chrono.h"
#include "rex_std/thread.h"
#include "rex_std/iostream.h"

#include <Windows.h>
#include <ShlObj.h>

TEST_CASE("Path Joining")
{
  CHECK(rex::path::join("D:", "foo", "bar") == "D:/foo/bar");
  CHECK(rex::path::join("foo", "bar") == "foo/bar");
  CHECK(rex::path::join("foo", "bar.txt") == "foo/bar.txt");
}

TEST_CASE("Change Extension")
{
  CHECK(rex::path::change_extension("foo.txt", ".bar") == "foo.bar");
  CHECK(rex::path::change_extension("foo.txt", "") == "foo");
  CHECK(rex::path::change_extension("foo", ".bar") == "foo.bar");
  CHECK(rex::path::change_extension("foo", "") == "foo");

  CHECK(rex::path::change_extension("bar/foo.txt", ".bar") == "bar/foo.bar");
  CHECK(rex::path::change_extension("bar/foo.txt", "") == "bar/foo");
  CHECK(rex::path::change_extension("bar/foo", ".bar") == "bar/foo.bar");
  CHECK(rex::path::change_extension("bar/foo", "") == "bar/foo");
}

TEST_CASE("Dir Name")
{
  CHECK(rex::path::dir_name("foo.txt") == "");
  CHECK(rex::path::dir_name("bar/foo.txt") == "bar");
}

TEST_CASE("Get Extension")
{
  CHECK(rex::path::extension("foo.txt") == ".txt");
  CHECK(rex::path::extension("foo.txt.bar") == ".txt.bar");
  CHECK(rex::path::extension("foo") == "");
  
  CHECK(rex::path::extension("bar/foo.txt") == ".txt");
  CHECK(rex::path::extension("bar/foo.txt.bar") == ".txt.bar");
  CHECK(rex::path::extension("bar/foo") == "");
}

TEST_CASE("Get Stem")
{
  CHECK(rex::path::stem("foo.txt") == "foo");
  CHECK(rex::path::stem("foo.txt.bar") == "foo");
  CHECK(rex::path::stem("foo") == "foo");

  CHECK(rex::path::stem("bar/foo.txt") == "foo");
  CHECK(rex::path::stem("bar/foo.txt.bar") == "foo");
  CHECK(rex::path::stem("bar/foo") == "foo");
}

TEST_CASE("Absolute Path")
{
  rsl::string cwd = rex::path::cwd();

  CHECK(rex::path::abs_path("foo.txt") == rex::path::join(cwd, "foo.txt"));
  CHECK(rex::path::abs_path("foo.txt.bar") == rex::path::join(cwd, "foo.txt.bar"));
  CHECK(rex::path::abs_path("foo") == rex::path::join(cwd, "foo"));

  CHECK(rex::path::abs_path("bar/foo.txt") == rex::path::join(cwd, "bar", "foo.txt"));
  CHECK(rex::path::abs_path("bar/foo.txt.bar") == rex::path::join(cwd, "bar", "foo.txt.bar"));
  CHECK(rex::path::abs_path("bar/foo") == rex::path::join(cwd, "bar", "foo"));

  CHECK(rex::path::abs_path("c:/bar/foo.txt") == "c:/bar/foo.txt");
  CHECK(rex::path::abs_path("c:/bar/foo.txt.bar") == "c:/bar/foo.txt.bar");
  CHECK(rex::path::abs_path("c:/bar/foo") == "c:/bar/foo");
}

TEST_CASE("Path Root")
{
  CHECK(rex::path::path_root("c:/bar/foo.txt") == "c:");
  CHECK(rex::path::path_root("bar/foo") == "");
  CHECK(rex::path::path_root("./bar/foo") == ""); // windows only, so root is empty
  CHECK(rex::path::path_root("/bar/foo") == ""); // windows only, so root is empty
  CHECK(rex::path::path_root("~/bar/foo") == ""); // windows only, so root is empty
}

TEST_CASE("Common Path")
{
  {
    rsl::vector<rsl::string_view> paths =
    {
      "c:/bar/foo.txt",
      "c:/bar/text.txt"
    };

    CHECK(rex::path::common_path(paths) == "c:/bar/");
  }

  {
    rsl::vector<rsl::string_view> paths =
    {
      "c:/foo/foo.txt",
      "c:/bar/text.txt"
    };

    CHECK(rex::path::common_path(paths) == "c:/");
  }

  {
    rsl::vector<rsl::string_view> paths =
    {
      "c:/bar/foo.txt",
      "d:/bar/text.txt"
    };

    CHECK(rex::path::common_path(paths) == "");
  }
}

TEST_CASE("Random Dir")
{
  for (int i = 0; i < 1000; ++i)
  {
    CHECK(!rex::path::exists(rex::path::random_dir()));
  }
}

TEST_CASE("Random Filename")
{
  for (int i = 0; i < 1000; ++i)
  {
    CHECK(!rex::path::exists(rex::path::random_filename()));
  }
}

TEST_CASE("Real Path")
{
  // Make sure that the working directory is set correctly here
  CHECK(rex::path::real_path("shortcut_to_foo.txt.lnk") == rex::path::abs_path("foo.txt"));  
}

TEST_CASE("Normalise Path")
{
  CHECK(rex::path::norm_path("/foo/./bar") == rex::path::join("foo", "bar"));
  CHECK(rex::path::norm_path("/path/to/dir") == rex::path::join("path","to","dir"));
  CHECK(rex::path::norm_path("path/to/dir") == rex::path::join("path","to","dir"));
  CHECK(rex::path::norm_path("/") == "");

  CHECK(rex::path::norm_path("/path/./to/./dir") == rex::path::join("path","to","dir"));
  CHECK(rex::path::norm_path("/path/to/../dir") == rex::path::join("path","dir"));
  CHECK(rex::path::norm_path("/path/to/../../dir") == "dir");

  CHECK(rex::path::norm_path("/path/to/dir\\file") == rex::path::join("path","to","dir","file"));
  CHECK(rex::path::norm_path("\\path\\to\\dir\\file") == rex::path::join("path", "to", "dir", "file"));
  CHECK(rex::path::norm_path("path/to/dir/file\\") == rex::path::join("path", "to", "dir", "file"));

  CHECK(rex::path::norm_path("//path/to//dir") == rex::path::join("path","to","dir"));
  CHECK(rex::path::norm_path("/\\path\\to/dir") == rex::path::join("path","to","dir"));
  CHECK(rex::path::norm_path("/path/to/dir\\") == rex::path::join("path","to","dir"));

  CHECK(rex::path::norm_path("/path//to//dir") == rex::path::join("path","to","dir"));
  CHECK(rex::path::norm_path("/path/to//dir") == rex::path::join("path","to","dir"));
  CHECK(rex::path::norm_path("/path/to/dir//") == rex::path::join("path","to","dir"));

  CHECK(rex::path::norm_path("/path//to//dir/") == rex::path::join("path","to","dir"));
  CHECK(rex::path::norm_path("../..") == rex::path::join("..",".."));
  CHECK(rex::path::norm_path("path/../..") == "..");

  CHECK(rex::path::norm_path("C:\\path\\to\\dir") == rex::path::join("C:","path","to","dir"));
  CHECK(rex::path::norm_path("C:\\path\\to\\..\\dir") == rex::path::join("C:","path","dir"));
}

TEST_CASE("Relative Path")
{
  CHECK(rex::path::rel_path("/path/to/dir", "/path") == rex::path::join("to", "dir"));
  CHECK(rex::path::rel_path("path/to/dir", "path") == rex::path::join("to", "dir"));
  CHECK(rex::path::rel_path("/", "/") == rex::path::join(""));

  CHECK(rex::path::rel_path("/path/./to/./dir", "/path") == rex::path::join("to", "dir"));
  CHECK(rex::path::rel_path("/path/to/../dir", "/path") == rex::path::join("dir"));
  CHECK(rex::path::rel_path("/path/to/../../dir", "/path") == rex::path::join("..", "dir"));

  CHECK(rex::path::rel_path("/path/to/dir\\file", "/path") == rex::path::join("to", "dir", "file"));
  CHECK(rex::path::rel_path("\\path\\to\\dir\\file", "/path") == rex::path::join("to", "dir", "file"));
  CHECK(rex::path::rel_path("path/to/dir/file\\", "path") == rex::path::join("to", "dir", "file"));

  CHECK(rex::path::rel_path("//path/to//dir", "/") == rex::path::join("path", "to", "dir"));
  CHECK(rex::path::rel_path("/\\path\\to/dir", "/") == rex::path::join("path", "to", "dir"));
  CHECK(rex::path::rel_path("/path/to/dir\\", "/") == rex::path::join("path", "to", "dir"));

  CHECK(rex::path::rel_path("/path//to//dir", "/") == rex::path::join("path", "to", "dir"));
  CHECK(rex::path::rel_path("/path/to//dir", "/") == rex::path::join("path", "to", "dir"));
  CHECK(rex::path::rel_path("/path/to/dir//", "/") == rex::path::join("path", "to", "dir"));

  CHECK(rex::path::rel_path("/path//to//dir/", "/") == rex::path::join("path", "to", "dir"));
  CHECK(rex::path::rel_path("/", "path/to/dir") == rex::path::join("..", "..", ".."));

  CHECK(rex::path::rel_path("C:\\path\\to\\dir", "C:\\path") == rex::path::join("to", "dir"));
  CHECK(rex::path::rel_path("C:\\path\\to\\..\\dir", "C:\\path") == rex::path::join("dir"));
}

TEST_CASE("File Creation Time")
{
  // get the current time
  auto now = rsl::chrono::high_resolution_clock::now().time_since_epoch();
  
  // create a file that doesn't exist yet
  rsl::string random_filename = rex::path::random_filename();

  // query the creation time and compare it to the original time with a threshold
  auto creation_time = rex::path::creation_time(random_filename);
  // No proper way atm to create or delete files, unless using the vfs
  rsl::cout << "COULDN'T TEST FILE CREATION TIME\n";

  // wait 1 tail
  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1s);
  
  // query the creation time again, it should still be the same
  creation_time = rex::path::creation_time(random_filename);
}

TEST_CASE("File Modification Time")
{
  // get the current time
  auto now = rsl::chrono::high_resolution_clock::now().time_since_epoch();

  // create a file that doesn't exist yet
  rsl::string random_filename = rex::path::random_filename();

  // query the modification time and compare it to the original time with a threshold
  auto creation_time = rex::path::modification_time(random_filename);
  // No proper way atm to create or delete files, unless using the vfs
  rsl::cout << "COULDN'T TEST FILE MODIFICATION TIME\n";

  // wait 1 tail
  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1s);

  // query the modification time again, it should still be the same
  creation_time = rex::path::modification_time(random_filename);

  // get the current time again
  now = rsl::chrono::high_resolution_clock::now().time_since_epoch();

  // Modify the file
  // No proper way atm to modify a file, unless using the vfs

  // query the modification time again, it should now be different
  creation_time = rex::path::modification_time(random_filename);
}

TEST_CASE("File Access Time")
{
  // get the current time
  auto now = rsl::chrono::high_resolution_clock::now().time_since_epoch();

  // create a file that doesn't exist yet
  rsl::string random_filename = rex::path::random_filename();

  // query the modification time and compare it to the original time with a threshold
  auto creation_time = rex::path::modification_time(random_filename);
  // No proper way atm to create or delete files, unless using the vfs
  rsl::cout << "COULDN'T TEST FILE ACCESS TIME\n";

  // wait 1 tail
  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1s);

  // query the access  time again, it should still be the same
  creation_time = rex::path::access_time(random_filename);

  // get the current time again
  now = rsl::chrono::high_resolution_clock::now().time_since_epoch();

  // Modify the file
  // No proper way atm to modify a file, unless using the vfs

  // query the access  time again, it should now be different
  creation_time = rex::path::access_time(random_filename);

  // get the current time again
  now = rsl::chrono::high_resolution_clock::now().time_since_epoch();

  // Read the file
  // No proper way atm to read a file, unless using the vfs

  // query the access time again, it should now be different
  creation_time = rex::path::access_time(random_filename);
}

TEST_CASE("File Size")
{
  CHECK(rex::path::file_size("file_0_bytes.txt") == 0);
  CHECK(rex::path::file_size("file_500_bytes.txt") == 500);
  CHECK(rex::path::file_size("file_1000_bytes.txt") == 1000);
}

TEST_CASE("Has Extension")
{
  CHECK(rex::path::has_extension("file.txt") == true);
  CHECK(rex::path::has_extension("document.pdf") == true);
  CHECK(rex::path::has_extension("/path/to/file.txt") == true);
  CHECK(rex::path::has_extension("/path/to/document.pdf") == true);

  CHECK(rex::path::has_extension("file") == false);
  CHECK(rex::path::has_extension("/path/to/file") == false);
  CHECK(rex::path::has_extension("no_extension.") == false);
  CHECK(rex::path::has_extension("/path/to/no_extension.") == false);

  CHECK(rex::path::has_extension("") == false);  // Empty string
  CHECK(rex::path::has_extension(".") == false);  // Only a dot, not a valid extension
  CHECK(rex::path::has_extension("..") == false); // Double dots, not a valid extension
}

TEST_CASE("Path Exists")
{
  CHECK(rex::path::exists("file_that_definitely_exists.txt"));
  CHECK(rex::path::exists(rex::path::random_filename()) == false);
}

TEST_CASE("Is Absolute")
{
    CHECK(rex::path::is_absolute("/absolute/path") == true);
    CHECK(rex::path::is_absolute("/another/absolute/path/file.txt") == true);
    CHECK(rex::path::is_absolute("C:\\Windows\\System32\\file.dll") == true);  // Windows path

    CHECK(rex::path::is_absolute("relative/path") == false);
    CHECK(rex::path::is_absolute("./relative/path") == false);
    CHECK(rex::path::is_absolute("../relative/path/file.txt") == false);

    CHECK(rex::path::is_absolute("") == false);  // Empty string
    CHECK(rex::path::is_absolute(".") == false);  // Current directory
    CHECK(rex::path::is_absolute("..") == false); // Parent directory
}

TEST_CASE("Is Relative")
{
    CHECK(rex::path::is_relative("relative/path") == true);
    CHECK(rex::path::is_relative("./relative/path") == true);
    CHECK(rex::path::is_relative("../relative/path/file.txt") == true);

    CHECK(rex::path::is_relative("/absolute/path") == false);
    CHECK(rex::path::is_relative("/another/absolute/path/file.txt") == false);
    CHECK(rex::path::is_relative("C:\\Windows\\System32\\file.dll") == false);  // Windows path

    CHECK(rex::path::is_relative("") == false);  // Empty string
    CHECK(rex::path::is_relative(".") == false);  // Current directory
    CHECK(rex::path::is_relative("..") == false); // Parent directory
}

TEST_CASE("Is File")
{
  CHECK(rex::path::is_file("this_is_a_file.txt"));
  CHECK(rex::path::is_file("this_is_a_directory") == false);
  CHECK(rex::path::is_file(rex::path::random_filename()) == false);
  CHECK(rex::path::is_file(rex::path::random_dir()) == false);
}

TEST_CASE("Is Directory")
{
  CHECK(rex::path::is_dir("this_is_a_file.txt") == false);
  CHECK(rex::path::is_dir("this_is_a_directory"));
  CHECK(rex::path::is_dir(rex::path::random_filename()) == false);
  CHECK(rex::path::is_dir(rex::path::random_dir()) == false);
}

TEST_CASE("Is Junction")
{
  CHECK(rex::path::is_junction("original_folder") == false);
  CHECK(rex::path::is_junction("original_file.txt") == false);
  CHECK(rex::path::is_junction("junction_folder") == true);
}

TEST_CASE("Is Link")
{
  CHECK(rex::path::is_link("original_folder") == false);
  CHECK(rex::path::is_link("original_file.txt") == false);
  CHECK(rex::path::is_link("junction_folder") == false);
}

TEST_CASE("Is Same File")
{
  CHECK(rex::path::same_path("same_file.txt", "same_file.txt") == true);
  CHECK(rex::path::same_path("sub_folder_1/../same_file.txt", "same_file.txt") == true);
  CHECK(rex::path::same_path("same_file.txt", rex::path::random_filename()) == false);
  CHECK(rex::path::same_path("same_file.txt", rex::path::random_dir()) == false);


  CHECK(rex::path::same_path("sub_folder_1", "sub_folder_1") == true);
  CHECK(rex::path::same_path("sub_folder_1/../sub_folder_1", "sub_folder_1") == true);
  CHECK(rex::path::same_path("same_file.txt", rex::path::random_filename()) == false);
  CHECK(rex::path::same_path("same_file.txt", rex::path::random_dir()) == false);

  CHECK(rex::path::same_path(rex::path::random_filename(), rex::path::random_filename()) == false);
  CHECK(rex::path::same_path(rex::path::random_filename(), rex::path::random_dir()) == false);
  CHECK(rex::path::same_path(rex::path::random_dir(), rex::path::random_dir()) == false);
}

TEST_CASE("Split")
{
    auto result = rex::path::split("/path/to/file.txt");
    CHECK(result.head == "/path/to");
    CHECK(result.tail == "file.txt");

    result = rex::path::split("C:\\Windows\\System32\\file.dll");
    CHECK(result.head == "C:\\Windows\\System32");
    CHECK(result.tail == "file.dll");

    result = rex::path::split("./relative/path");
    CHECK(result.head == "./relative");
    CHECK(result.tail == "path");

    result = rex::path::split("");
    CHECK(result.head == "");
    CHECK(result.tail == "");

    result = rex::path::split(".");
    CHECK(result.head == "");
    CHECK(result.tail == ".");

    result = rex::path::split("..");
    CHECK(result.head == "");
    CHECK(result.tail == "..");
}

TEST_CASE("Split Origin")
{
    auto result = rex::path::split_origin("C:\\Windows\\System32\\file.dll");
    CHECK(result.head == "C:");
    CHECK(result.tail == "Windows\\System32\\file.dll");

    result = rex::path::split_origin("\\Windows\\System32\\file.dll");
    CHECK(result.head == "");
    CHECK(result.tail == "\\Windows\\System32\\file.dll");

    result = rex::path::split_origin("/path/to/file.txt");
    CHECK(result.head == "");
    CHECK(result.tail == "/path/to/file.txt");

    result = rex::path::split_origin("");
    CHECK(result.head == "");
    CHECK(result.tail == "");

    result = rex::path::split_origin("./relative/path");
    CHECK(result.head == "");
    CHECK(result.tail == "./relative/path");
}

TEST_CASE("Split Extension")
{
    auto result = rex::path::split_ext("/path/to/file.txt");
    CHECK(result.head == "/path/to/file");
    CHECK(result.tail == ".txt");

    result = rex::path::split_ext("C:\\Windows\\System32\\file.dll");
    CHECK(result.head == "C:\\Windows\\System32\\file");
    CHECK(result.tail == ".dll");

    result = rex::path::split_ext("./relative/path/file.txt");
    CHECK(result.head == "./relative/path/file");
    CHECK(result.tail == ".txt");

    result = rex::path::split_ext("/path/to/file");
    CHECK(result.head == "/path/to/file");
    CHECK(result.tail == "");

    result = rex::path::split_ext("C:\\Windows\\System32\\file");
    CHECK(result.head == "C:\\Windows\\System32\\file");
    CHECK(result.tail == "");

    result = rex::path::split_ext("./relative/path/file");
    CHECK(result.head == "./relative/path/file");
    CHECK(result.tail == "");

    result = rex::path::split_ext("");
    CHECK(result.head == "");
    CHECK(result.tail == "");

    result = rex::path::split_ext(".");
    CHECK(result.head == ".");
    CHECK(result.tail == "");

    result = rex::path::split_ext("..");
    CHECK(result.head == "..");
    CHECK(result.tail == "");

    result = rex::path::split_ext("./.");
    CHECK(result.head == "./.");
    CHECK(result.tail == "");

    result = rex::path::split_ext("../..");
    CHECK(result.head == "../..");
    CHECK(result.tail == "");
}

TEST_CASE("Split Root")
{
    auto result = rex::path::split_root("C:\\Windows\\System32\\file.dll");
    CHECK(result.drive == "C:");
    CHECK(result.root == "\\");
    CHECK(result.tail == "Windows\\System32\\file.dll");

    result = rex::path::split_root("\\Windows\\System32\\file.dll");
    CHECK(result.drive == "");
    CHECK(result.root == "\\");
    CHECK(result.tail == "Windows\\System32\\file.dll");

    result = rex::path::split_root("/path/to/file.txt");
    CHECK(result.drive == "");
    CHECK(result.root == "/");
    CHECK(result.tail == "path/to/file.txt");

    result = rex::path::split_root("relative/path/file.txt");
    CHECK(result.drive == "");
    CHECK(result.root == "");
    CHECK(result.tail == "relative/path/file.txt");

    result = rex::path::split_root("");
    CHECK(result.drive == "");
    CHECK(result.root == "");
    CHECK(result.tail == "");

    result = rex::path::split_root("./relative/path");
    CHECK(result.drive == "");
    CHECK(result.root == "");
    CHECK(result.tail == "./relative/path");
}