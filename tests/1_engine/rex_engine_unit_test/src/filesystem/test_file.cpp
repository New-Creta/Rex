#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "rex_std/thread.h"

TEST_CASE("File Creation Time")
{
  // create a file that doesn't exist yet
  rsl::string random_filename = rex::path::random_filename();
  random_filename = rex::path::abs_path(random_filename);
  rex::file::create(random_filename);

  // query the creation time and compare it to the original time with a threshold
  auto creation_time = rex::file::creation_time(random_filename);

  // wait 1 tail
  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1s);

  // query the creation time again, it should still be the same
  auto new_creation_time = rex::file::creation_time(random_filename);

  REX_CHECK(creation_time == new_creation_time);

  // delete the file to leave nothing behind
  rex::file::del(random_filename);
}

TEST_CASE("File Modification Time")
{
  // create a file that doesn't exist yet
  rsl::string random_filename = rex::path::random_filename();
  random_filename = rex::path::abs_path(random_filename);
  rex::file::create(random_filename);

  // query the modification time and compare it to the original time with a threshold
  auto mod_time = rex::file::modification_time(random_filename);

  // wait 1 tail
  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1s);

  // query the modification time again, it should still be the same
  auto new_mod_time = rex::file::modification_time(random_filename);
  REX_CHECK(mod_time == new_mod_time);

  // Modify the file
  rex::file::append_text(random_filename, "random_text.txt");

  // query the modification time again, it should be different now
  new_mod_time = rex::file::modification_time(random_filename);
  REX_CHECK(mod_time != new_mod_time);

  // delete the file to leave nothing behind
  rex::file::del(random_filename);
}

TEST_CASE("File Access Time")
{
  // get the current time
  auto now = rsl::chrono::high_resolution_clock::now().time_since_epoch();

  // create a file that doesn't exist yet
  rsl::string random_filename = rex::path::random_filename();
  random_filename = rex::path::abs_path(random_filename);
  rex::file::create(random_filename);

  // query the modification time and compare it to the original time with a threshold
  auto access_time = rex::file::modification_time(random_filename);

  // wait 1 tail
  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1s);

  // query the access  time again, it should still be the same
  auto new_access_time = rex::file::access_time(random_filename);

  // Modify the file
  rex::file::append_text(random_filename, "random_text.txt");

  // query the access  time again, it should now be different
  new_access_time = rex::file::access_time(random_filename);

  // access time should now be different
  REX_CHECK(access_time != new_access_time);

  // query an attribute of the file, attribute changes don't affect the access time
  rex::file::is_readonly(random_filename);

  auto newer_access_time = rex::file::access_time(random_filename);

  // access time should now be different
  REX_CHECK(access_time != newer_access_time);
  REX_CHECK(new_access_time == newer_access_time);

  // delete the file to leave nothing behind
  rex::file::del(random_filename);
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
  REX_CHECK(rex::file::exists(rex::path::random_filename()) == false);
  REX_CHECK(rex::file::exists("this_is_a_file.txt"));
  REX_CHECK(rex::file::exists("this_is_a_directory") == false);
  REX_CHECK(rex::file::exists(rex::path::random_filename()) == false);
  REX_CHECK(rex::file::exists(rex::path::random_dir()) == false);
}