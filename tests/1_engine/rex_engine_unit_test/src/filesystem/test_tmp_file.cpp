#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/tmp_file.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"

TEST_CASE("TEST - Temp File - Empty")
{
  rsl::string filename;
  {
    rex::TempFile tmp_file;
    filename.assign(tmp_file.filepath());
    
    REX_CHECK(rex::file::exists(filename) == true);
    REX_CHECK(rex::path::is_under_dir(filename, rex::path::cwd()) == true);
  }
  REX_CHECK(rex::file::exists(filename) == false);

  rsl::string test_directory = rex::path::abs_path("test_directory");
  rex::directory::create(test_directory);
  {

    rex::TempFile tmp_file(test_directory);
    filename.assign(tmp_file.filepath());

    REX_CHECK(rex::file::exists(filename) == true);
    REX_CHECK(rex::path::is_under_dir(filename, rex::path::cwd()) == true);
    REX_CHECK(rex::path::is_under_dir(filename, test_directory) == true);

  }
  rex::directory::del(test_directory);
  REX_CHECK(rex::file::exists(filename) == false);
}

TEST_CASE("TEST - Temp File - With Text")
{
  rsl::string filename;
  {
    rex::TempFile tmp_file;
    filename.assign(tmp_file.filepath());

    int x = 1;
    tmp_file.write(&x, sizeof(x));

    rex::memory::Blob content = tmp_file.read();
    int y = content.read<int>();

    REX_CHECK(x == y);
  }
  REX_CHECK(rex::file::exists(filename) == false);

  rsl::string test_directory = rex::path::abs_path("test_directory");
  rex::directory::create(test_directory);

  {
    rex::TempFile tmp_file(test_directory);
    filename.assign(tmp_file.filepath());

    int x = 1;
    tmp_file.write(&x, sizeof(x));

    rex::memory::Blob content = tmp_file.read();
    int y = content.read<int>();

    REX_CHECK(x == y);

  }
  rex::directory::del(test_directory);
  REX_CHECK(rex::file::exists(filename) == false);

}