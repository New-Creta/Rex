#include "rex_test/catch2/catch.hpp"

#include "rex_std/iostream.h"

#include "rex_engine/filesystem/path.h"

#include <Windows.h>
#include <ShlObj.h>

#include "rex_std/iostream.h"

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

TEST_CASE("Real Path")
{
  CHECK(rex::path::real_path("shortcut_to_foo.txt.lnk") == rex::path::abs_path("foo.txt"));  
}

TEST_CASE("Normalise Path")
{
}

TEST_CASE("Relative Path")
{
}

TEST_CASE("File Creation Time")
{
}

TEST_CASE("File Modification Time")
{
}

TEST_CASE("File Access Time")
{
}

TEST_CASE("File Size")
{
}

TEST_CASE("Has Extension")
{
}

TEST_CASE("Path Exists")
{
}

TEST_CASE("Is Absolute")
{
}

TEST_CASE("Is Relative")
{
}

TEST_CASE("Is File")
{
}

TEST_CASE("Is Directory")
{
}

TEST_CASE("Is Junction")
{
}

TEST_CASE("Is Link")
{
}

TEST_CASE("Is Same File")
{
}

TEST_CASE("Is Same Directory")
{
}

TEST_CASE("Split")
{
}

TEST_CASE("Split Drive")
{
}

TEST_CASE("Split Extension")
{
}

TEST_CASE("Split Root")
{
}