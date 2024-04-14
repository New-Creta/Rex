#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/file.h"

TEST_CASE("Directory Exists")
{
  REX_CHECK(rex::directory::exists("this_is_a_file.txt") == false);
  REX_CHECK(rex::directory::exists("this_is_a_directory"));
  REX_CHECK(rex::directory::exists(rex::path::random_filename()) == false);
  REX_CHECK(rex::directory::exists(rex::path::random_dir()) == false);
}

TEST_CASE("List Dirs")
{
  auto dirs = rex::directory::list_dirs("list_dir");

  REX_CHECK(rsl::find_if(dirs.cbegin(), dirs.cend(), [](rsl::string_view dir) {return rex::path::same_path(dir, rex::path::join("list_dir", "folder1")); }) != dirs.cend());
  REX_CHECK(rsl::find_if(dirs.cbegin(), dirs.cend(), [](rsl::string_view dir) {return rex::path::same_path(dir, rex::path::join("list_dir", "folder2")); }) != dirs.cend());
  REX_CHECK(rsl::find_if(dirs.cbegin(), dirs.cend(), [](rsl::string_view dir) {return rex::path::same_path(dir, rex::path::join("list_dir", "folder3")); }) != dirs.cend());
}

TEST_CASE("List Files")
{
  auto files = rex::directory::list_files("list_dir");

  REX_CHECK(rsl::find_if(files.cbegin(), files.cend(), [](rsl::string_view file) {return rex::path::same_path(file, rex::path::join("list_dir", "file1.txt")); }) != files.cend());
  REX_CHECK(rsl::find_if(files.cbegin(), files.cend(), [](rsl::string_view file) {return rex::path::same_path(file, rex::path::join("list_dir", "file2.txt")); }) != files.cend());
  REX_CHECK(rsl::find_if(files.cbegin(), files.cend(), [](rsl::string_view file) {return rex::path::same_path(file, rex::path::join("list_dir", "file3.txt")); }) != files.cend());

}

TEST_CASE("List Entries")
{
  auto entries = rex::directory::list_entries("list_dir");

  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "folder1")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "folder2")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "folder3")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "file1.txt")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "file2.txt")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "file3.txt")); }) != entries.cend());
}

TEST_CASE("List Entries Recursive")
{
  auto entries = rex::directory::list_entries("list_dir", rex::directory::ListRecusrive::yes);

  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "folder1")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "folder2")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "folder3")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "file1.txt")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "file2.txt")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "file3.txt")); }) != entries.cend());
  REX_CHECK(rsl::find_if(entries.cbegin(), entries.cend(), [](rsl::string_view entry) {return rex::path::same_path(entry, rex::path::join("list_dir", "folder1", "sub_file.txt")); }) != entries.cend());
}

TEST_CASE("Recursive Deletion")
{
  rex::directory::create("root");
  rex::directory::create(rex::path::join("root", "folder"));
  rex::file::create(rex::path::join("root", "folder", "file.txt"));

  rex::directory::del_recusrive("root");

  REX_CHECK(rex::directory::exists("root") == false);
}