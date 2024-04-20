#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/tmp_dir.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/path.h"

TEST_CASE("Empty Temp Dir")
{
  rsl::string dirname;
  {
    rex::TempDirectory tmp_dir{};
    dirname.assign(tmp_dir.dirname());
    REX_CHECK(rex::directory::exists(dirname) == true);
    REX_CHECK(rex::path::is_under_dir(dirname, rex::path::cwd()));
  }
  REX_CHECK(rex::directory::exists(dirname) == false);

  rsl::string test_directory = rex::path::abs_path("test_directory");
  rex::directory::create(test_directory);
  {
    rex::TempDirectory tmp_dir(test_directory);
    dirname.assign(tmp_dir.dirname());
    REX_CHECK(rex::directory::exists(dirname) == true);
    REX_CHECK(rex::path::is_under_dir(dirname, rex::path::cwd()) == true);
    REX_CHECK(rex::path::is_under_dir(dirname, test_directory) == true);

  }
  rex::directory::del(test_directory);
  REX_CHECK(rex::directory::exists(dirname) == false);
}

TEST_CASE("Temp Dir With Files")
{
  rsl::string random_file1 = rex::path::random_filename();
  rsl::string random_file2 = rex::path::random_filename();
  rsl::string dirname;
  {
    rex::TempDirectory tmp_dir{};
    dirname.assign(tmp_dir.dirname());

    tmp_dir.create_file(random_file1);
    tmp_dir.create_file(random_file2);

    REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file1)) == true);
    REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file2)) == true);
  }
  REX_CHECK(rex::directory::exists(dirname) == false);
  REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file1)) == false);
  REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file2)) == false);

  rsl::string test_directory = rex::path::abs_path("test_directory");
  rex::directory::create(test_directory);
  {

    rex::TempDirectory tmp_dir(test_directory);
    dirname.assign(tmp_dir.dirname());

    tmp_dir.create_file(random_file1);
    tmp_dir.create_file(random_file2);

    REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file1)) == true);
    REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file2)) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_file1), rex::path::cwd()) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_file1), test_directory) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_file2), rex::path::cwd()) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_file2), test_directory) == true);

  }
  rex::directory::del(test_directory);
  REX_CHECK(rex::directory::exists(dirname) == false);
  REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file1)) == false);
  REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file2)) == false);
}

TEST_CASE("Temp Dir With Directories")
{
  rsl::string dirname;
  rsl::string random_dir1 = rex::path::random_dir();
  rsl::string random_dir2 = rex::path::random_dir();
  {
    rex::TempDirectory tmp_dir{};
    dirname.assign(tmp_dir.dirname());

    tmp_dir.create_dir(random_dir1);
    tmp_dir.create_dir(random_dir2);

    REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir1)) == true);
    REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir2)) == true);
  }
  REX_CHECK(rex::directory::exists(dirname) == false);
  REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir1)) == false);
  REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir2)) == false);

  rsl::string test_directory = rex::path::abs_path("test_directory");
  rex::directory::create(test_directory);
  {

    rex::TempDirectory tmp_dir(test_directory);
    dirname.assign(tmp_dir.dirname());

    tmp_dir.create_dir(random_dir1);
    tmp_dir.create_dir(random_dir2);

    REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir1)) == true);
    REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir2)) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_dir1), rex::path::cwd()) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_dir1), test_directory) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_dir2), rex::path::cwd()) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_dir2), test_directory) == true);

  }
  rex::directory::del(test_directory);
  REX_CHECK(rex::directory::exists(dirname) == false);
  REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir1)) == false);
  REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir2)) == false);
}

TEST_CASE("Temp Dir With Files And Directories")
{
  rsl::string random_file1 = rex::path::random_filename();
  rsl::string random_file2 = rex::path::random_filename();

  rsl::string random_dir1 = rex::path::random_dir();
  rsl::string random_dir2 = rex::path::random_dir();

  rsl::string dirname;
  {
    rex::TempDirectory tmp_dir{};
    dirname.assign(tmp_dir.dirname());

    tmp_dir.create_file(random_file1);
    tmp_dir.create_file(random_file2);

    tmp_dir.create_dir(random_dir1);
    tmp_dir.create_dir(random_dir2);

    REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file1)) == true);
    REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file2)) == true);
    REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir1)) == true);
    REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir2)) == true);
  }
  REX_CHECK(rex::directory::exists(dirname) == false);
  REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file1)) == false);
  REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file2)) == false);
  REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir1)) == false);
  REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir2)) == false);

  rsl::string test_directory = rex::path::abs_path("test_directory");
  rex::directory::create(test_directory);
  {
    rex::TempDirectory tmp_dir(test_directory);
    dirname.assign(tmp_dir.dirname());

    tmp_dir.create_file(random_file1);
    tmp_dir.create_file(random_file2);

    tmp_dir.create_dir(random_dir1);
    tmp_dir.create_dir(random_dir2);

    tmp_dir.create_file(rex::path::join(random_dir1, random_file1));
    tmp_dir.create_file(rex::path::join(random_dir2, random_file2));

    REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file1)) == true);
    REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file2)) == true);
    REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir1)) == true);
    REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir2)) == true);
    REX_CHECK(rex::file::exists(rex::path::join(dirname, rex::path::join(random_dir1, random_file1))) == true);
    REX_CHECK(rex::file::exists(rex::path::join(dirname, rex::path::join(random_dir2, random_file2))) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_file1), rex::path::cwd()) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_file1), test_directory) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_file2), rex::path::cwd()) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_file2), test_directory) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_dir1), rex::path::cwd()) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_dir1), test_directory) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_dir2), rex::path::cwd()) == true);
    REX_CHECK(rex::path::is_under_dir(rex::path::join(dirname, random_dir2), test_directory) == true);

  }
  rex::directory::del(test_directory);
  REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file1)) == false);
  REX_CHECK(rex::file::exists(rex::path::join(dirname, random_file2)) == false);
  REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir1)) == false);
  REX_CHECK(rex::directory::exists(rex::path::join(dirname, random_dir2)) == false);
  REX_CHECK(rex::file::exists(rex::path::join(dirname, rex::path::join(random_dir1, random_file1))) == false);
  REX_CHECK(rex::file::exists(rex::path::join(dirname, rex::path::join(random_dir2, random_file2))) == false);
}