#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/tmp_file.h"

#include "rex_std/thread.h"

TEST_CASE("File Creation Time")
{
  // create a temp file, so it automatically cleans up
  rex::TempFile tmp_file;

  // query the creation time and compare it to the original time with a threshold
  auto creation_time = rex::file::creation_time(tmp_file.filename());

  // wait 1 tail
  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1s);

  // query the creation time again, it should still be the same
  auto new_creation_time = rex::file::creation_time(tmp_file.filename());

  REX_CHECK(creation_time == new_creation_time);
}

TEST_CASE("File Modification Time")
{
  // create a temp file, so it automatically cleans up
  rex::TempFile tmp_file;

  // query the modification time and compare it to the original time with a threshold
  auto mod_time = rex::file::modification_time(tmp_file.filename());

  // wait 1 tail
  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1s);

  // query the modification time again, it should still be the same
  auto new_mod_time = rex::file::modification_time(tmp_file.filename());
  REX_CHECK(mod_time == new_mod_time);

  // Modify the file
  rex::file::append_text(tmp_file.filename(), "random text");

  // query the modification time again, it should be different now
  new_mod_time = rex::file::modification_time(tmp_file.filename());
  REX_CHECK(mod_time != new_mod_time);
}

TEST_CASE("File Access Time")
{
  // create a temp file, so it automatically cleans up
  rex::TempFile tmp_file;

  // query the modification time and compare it to the original time with a threshold
  auto access_time = rex::file::modification_time(tmp_file.filename());

  // wait 1 tail
  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1s);

  // query the access  time again, it should still be the same
  auto new_access_time = rex::file::access_time(tmp_file.filename());

  // Modify the file
  rex::file::append_text(tmp_file.filename(), "random text");

  // query the access  time again, it should now be different
  new_access_time = rex::file::access_time(tmp_file.filename());

  // access time should now be different
  REX_CHECK(access_time != new_access_time);

  // query an attribute of the file, attribute changes don't affect the access time
  rex::file::is_readonly(tmp_file.filename());

  auto newer_access_time = rex::file::access_time(tmp_file.filename());

  // access time should now be different
  REX_CHECK(access_time != newer_access_time);
  REX_CHECK(new_access_time == newer_access_time);
}

TEST_CASE("File Size")
{
  REX_CHECK(rex::file::size("file_0_bytes.txt") == 0);
  REX_CHECK(rex::file::size("file_500_bytes.txt") == 500);
  REX_CHECK(rex::file::size("file_1000_bytes.txt") == 1000);
}

TEST_CASE("File Exists")
{
  REX_CHECK(rex::file::exists("file_that_definitely_exists.txt"));
  REX_CHECK(rex::file::exists("this_is_a_file.txt"));
  REX_CHECK(rex::file::exists("this_is_a_directory") == false);
  REX_CHECK(rex::file::exists(rex::path::random_filename()) == false);
  REX_CHECK(rex::file::exists(rex::path::random_dir()) == false);
}