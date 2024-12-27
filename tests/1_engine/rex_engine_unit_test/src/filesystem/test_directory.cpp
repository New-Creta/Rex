#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/tmp_cwd.h"
#include "rex_engine/filesystem/tmp_dir.h"

TEST_CASE("TEST - Directory - Exists")
{
  rex::TempCwd tmp_cwd("directory_tests");

  REX_CHECK(rex::directory::exists("this_is_a_file.txt") == false);
  REX_CHECK(rex::directory::exists("this_is_a_directory"));
  REX_CHECK(rex::directory::exists(rex::path::random_filename()) == false);
  REX_CHECK(rex::directory::exists(rex::path::random_dir()) == false);
}

TEST_CASE("TEST - Directory - List Dirs")
{
  rex::TempCwd tmp_cwd("directory_tests");

  auto dirs = rex::directory::list_dirs("list_dir");

  REX_CHECK(rsl::find_if(dirs.cbegin(), dirs.cend(), [](rsl::string_view dir) {return rex::path::is_same(dir, rex::path::join("list_dir", "folder1")); }) != dirs.cend());
  REX_CHECK(rsl::find_if(dirs.cbegin(), dirs.cend(), [](rsl::string_view dir) {return rex::path::is_same(dir, rex::path::join("list_dir", "folder2")); }) != dirs.cend());
  REX_CHECK(rsl::find_if(dirs.cbegin(), dirs.cend(), [](rsl::string_view dir) {return rex::path::is_same(dir, rex::path::join("list_dir", "folder3")); }) != dirs.cend());
}

TEST_CASE("TEST - Directory - List Files")
{
  rex::TempCwd tmp_cwd("directory_tests");

  auto files = rex::directory::list_files("list_dir");

  REX_CHECK(rsl::find_if(files.cbegin(), files.cend(), [](rsl::string_view file) {return rex::path::is_same(file, rex::path::join("list_dir", "file1.txt")); }) != files.cend());
  REX_CHECK(rsl::find_if(files.cbegin(), files.cend(), [](rsl::string_view file) {return rex::path::is_same(file, rex::path::join("list_dir", "file2.txt")); }) != files.cend());
  REX_CHECK(rsl::find_if(files.cbegin(), files.cend(), [](rsl::string_view file) {return rex::path::is_same(file, rex::path::join("list_dir", "file3.txt")); }) != files.cend());

}

TEST_CASE("TEST - Directory - List Entries")
{
  rex::TempCwd tmp_cwd("directory_tests");

  auto entries = rex::directory::list_entries("list_dir");

  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "folder1")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "folder2")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "folder3")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "file1.txt")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "file2.txt")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "file3.txt")); }) != entries.cend());
}

TEST_CASE("TEST - Directory - List Entries Recursive")
{
  rex::TempCwd tmp_cwd("directory_tests");

  auto entries = rex::directory::list_entries("list_dir", rex::directory::Recursive::yes);

  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "folder1")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "folder2")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "folder3")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "file1.txt")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "file2.txt")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "file3.txt")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::is_same(entry, rex::path::join("list_dir", "folder1", "sub_file.txt")); }) != entries.cend());
}

TEST_CASE("TEST - Directory - Recursive Deletion")
{
  rex::TempCwd tmp_cwd("directory_tests");

  rex::directory::create("root");
  rex::directory::create(rex::path::join("root", "folder"));
  rex::file::create(rex::path::join("root", "folder", "file.txt"));

  rex::directory::del_recursive("root");

  REX_CHECK(rex::directory::exists("root") == false);
}

TEST_CASE("TEST - Directory - Create and Deletion")
{
  rex::TempCwd tmp_cwd("directory_tests");

  rsl::string random_dirname = rex::path::random_dir();
  rex::Error error = rex::Error::no_error();

  // Create a random empty directory
  REX_CHECK(rex::directory::exists(random_dirname) == false);

  error = rex::directory::create(random_dirname);
  REX_CHECK(rex::directory::exists(random_dirname) == true);
  REX_CHECK(error.has_error() == false);
  error = rex::directory::del(random_dirname);

  REX_CHECK(rex::directory::exists(random_dirname) == false);
  REX_CHECK(error.has_error() == false);

  // Create a random non-empty directory
  random_dirname = rex::path::random_dir();
  REX_CHECK(rex::directory::exists(random_dirname) == false);

  error = rex::directory::create(random_dirname);
  REX_CHECK(rex::directory::exists(random_dirname) == true);
  REX_CHECK(error.has_error() == false);

  rsl::string random_sub_directory = rex::path::join(random_dirname, rex::path::random_dir());
  error = rex::directory::create(random_sub_directory);
  REX_CHECK(rex::directory::exists(random_sub_directory) == true);
  REX_CHECK(error.has_error() == false);

  // This should fail as the directory is not empty
  error = rex::directory::del(random_dirname);
  REX_CHECK(rex::directory::exists(random_dirname) == true);
  REX_CHECK(rex::directory::exists(random_sub_directory) == true);
  REX_CHECK(error.has_error() == true);

  // This should now succeed
  error = rex::directory::del_recursive(random_dirname);

  REX_CHECK(rex::directory::exists(random_dirname) == false);
  REX_CHECK(rex::directory::exists(random_sub_directory) == false);
  REX_CHECK(error.has_error() == false);

  // Do a recursive deletion
}

TEST_CASE("TEST - Directory - Copy")
{
  rex::TempCwd tmp_cwd("directory_tests");

  rsl::string random_dirname = rex::path::random_dir();
  REX_CHECK(rex::directory::exists(random_dirname) == false);

  rex::directory::create(random_dirname);
  REX_CHECK(rex::directory::exists(random_dirname) == true);
  
  rsl::string random_dirname2 = rex::path::random_dir();
  rex::directory::copy(random_dirname, random_dirname2);

  REX_CHECK(rex::directory::exists(random_dirname) == true);
  REX_CHECK(rex::directory::exists(random_dirname2) == true);

  rex::directory::del(random_dirname);
  rex::directory::del(random_dirname2);

  REX_CHECK(rex::directory::exists(random_dirname) == false);
  REX_CHECK(rex::directory::exists(random_dirname2) == false);
}

TEST_CASE("TEST - Directory - Move")
{
  rex::TempCwd tmp_cwd("directory_tests");

  rsl::string random_dirname = rex::path::random_dir();
  REX_CHECK(rex::directory::exists(random_dirname) == false);

  rex::directory::create(random_dirname);
  REX_CHECK(rex::directory::exists(random_dirname) == true);

  rsl::string random_dirname2 = rex::path::random_dir();
  rex::directory::move(random_dirname, random_dirname2);

  REX_CHECK(rex::directory::exists(random_dirname) == false);
  REX_CHECK(rex::directory::exists(random_dirname2) == true);

  rex::directory::del(random_dirname2);

  REX_CHECK(rex::directory::exists(random_dirname) == false);
  REX_CHECK(rex::directory::exists(random_dirname2) == false);
}

TEST_CASE("TEST - Directory - Size")
{
  rex::TempCwd tmp_cwd("directory_tests");

  {
    rsl::memory_size dir_size = rex::directory::size("dir_with_size_of_10_bytes_non_recursive", rex::directory::Recursive::no);
    REX_CHECK(dir_size == 10_bytes);
  }

  {
    rsl::memory_size dir_size = rex::directory::size("dir_with_size_of_100_bytes_recursive", rex::directory::Recursive::yes);
    REX_CHECK(dir_size == 100_bytes);
  }

  {
    rsl::memory_size dir_size = rex::directory::size(rex::path::random_dir());
    REX_CHECK(dir_size == 0_bytes);
  }
}

TEST_CASE("TEST - Directory - Is Empty")
{
  rex::TempCwd tmp_cwd("directory_tests");
  rex::TempDirectory tmp_dir("emppty_dir");

  REX_CHECK(rex::directory::is_empty("non_existing_dir") == true);
  REX_CHECK(rex::directory::is_empty("empty_dir") == true);
  REX_CHECK(rex::directory::is_empty("non_empty_dir1") == false);
  REX_CHECK(rex::directory::is_empty("non_empty_dir2") == false);
}