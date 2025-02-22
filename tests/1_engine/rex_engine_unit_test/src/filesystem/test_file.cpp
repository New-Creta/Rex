#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/tmp_file.h"
#include "rex_engine/filesystem/tmp_cwd.h"
#include "rex_engine/text_processing/text_processing.h"

#include "rex_std/thread.h"

TEST_CASE("TEST - File - Creation Time")
{
  rex::TempCwd tmp_cwd("file_tests");

  // create a temp file, so it automatically cleans up
  rex::TempFile tmp_file;

  // query the creation time and compare it to the original time with a threshold
  auto creation_time = rex::file::creation_time(tmp_file.filename());

  // query the creation time again, it should still be the same
  auto new_creation_time = rex::file::creation_time(tmp_file.filename());

  REX_CHECK(creation_time == new_creation_time);
}

TEST_CASE("TEST - File - Modification Time")
{
  rex::TempCwd tmp_cwd("file_tests");

  // create a temp file, so it automatically cleans up
  rex::TempFile tmp_file;

  // query the modification time and compare it to the original time with a threshold
  auto mod_time = rex::file::modification_time(tmp_file.filename());

  // query the modification time again, it should still be the same
  auto new_mod_time = rex::file::modification_time(tmp_file.filename());
  REX_CHECK(mod_time == new_mod_time);

  // Modify the file
  rex::file::append_text(tmp_file.filename(), "random text");

  // query the modification time again, it should be different now
  new_mod_time = rex::file::modification_time(tmp_file.filename());
  REX_CHECK(mod_time <= new_mod_time);
}

TEST_CASE("TEST - File - Access Time")
{
  rex::TempCwd tmp_cwd("file_tests");

  // Note: We cannot reliable say that access time of files will be different between checks
  // As it's determined by the OS when this gets updated. All we can say that between access
  // the newer access time is not before the original 
   
  // create a temp file, so it automatically cleans up
  rex::TempFile tmp_file;

  // query the access time and compare it to the original time with a threshold
  auto access_time = rex::file::access_time(tmp_file.filename());

  // query the access  time again, it should still be the same
  auto new_access_time = rex::file::access_time(tmp_file.filename());
  REX_CHECK(access_time == new_access_time);

  // Modify the file
  rex::file::append_text(tmp_file.filename(), "random text");

  // query the access  time again, it should now be different
  new_access_time = rex::file::access_time(tmp_file.filename());

  // access time should now be different
  REX_CHECK(access_time <= new_access_time);

  // query an attribute of the file, attribute changes don't affect the access time
  rex::file::is_readonly(tmp_file.filename());

  auto newer_access_time = rex::file::access_time(tmp_file.filename());

  // access time should now be different
  REX_CHECK(access_time <= newer_access_time);
  REX_CHECK(new_access_time == newer_access_time);
}

TEST_CASE("TEST - File - Size")
{
  rex::TempCwd tmp_cwd("file_tests");

  REX_CHECK(rex::file::size("file_0_bytes.txt") == 0);
  REX_CHECK(rex::file::size("file_500_bytes.txt") == 500);
  REX_CHECK(rex::file::size("file_1000_bytes.txt") == 1000);
}

TEST_CASE("TEST - File - Exists")
{
  rex::TempCwd tmp_cwd("file_tests");

  REX_CHECK(rex::file::exists("file_that_definitely_exists.txt"));
  REX_CHECK(rex::file::exists("this_is_a_file.txt"));
  REX_CHECK(rex::file::exists("this_is_a_directory") == false);
  REX_CHECK(rex::file::exists(rex::path::random_filename()) == false);
  REX_CHECK(rex::file::exists(rex::path::random_dir()) == false);
}

TEST_CASE("TEST - File - Reading")
{
  rex::TempCwd tmp_cwd("file_tests");

  {
    const rex::memory::Blob file_content = rex::file::read_file("test_file.txt");
    const rsl::string_view file_content_as_string = rex::memory::blob_to_string_view(file_content);

    REX_CHECK(file_content.data() != nullptr);
    REX_CHECK(file_content.size() == 26);
    REX_CHECK(file_content_as_string == "this is some dummy content");
  }

  {
    const rex::memory::Blob file_content = rex::file::read_file("empty_file.txt");

    REX_CHECK(file_content.data() == nullptr);
    REX_CHECK(file_content.size() == 0);
  }

  {
    const rex::memory::Blob file_content = rex::file::read_file("file_that_doesnt_exist.txt");

    REX_CHECK(file_content.data() == nullptr);
    REX_CHECK(file_content.size() == 0);
  }
}

TEST_CASE("TEST - File - Saving")
{
  rex::TempCwd tmp_cwd("file_tests");

  rsl::string_view dummy_content = "this is some dummy content";
  rex::TempString dummy_file = rex::path::random_filename();
  rex::file::write_to_file(dummy_file, dummy_content.data(), dummy_content.length());

  const rex::memory::Blob file_data = rex::file::read_file(dummy_file);
  const rsl::string_view file_content = rex::memory::blob_to_string_view(file_data);
  REX_CHECK(rex::file::size(dummy_file) == dummy_content.length());
  REX_CHECK(file_content == dummy_content);

  rex::file::del(dummy_file);
}

TEST_CASE("TEST - File - Append Line")
{
  rex::TempCwd tmp_cwd("file_tests");

  rsl::string_view dummy_content = "this is some dummy content";
  rex::TempString dummy_file = rex::path::random_filename();
  rex::file::append_line(dummy_file, dummy_content);

  rex::memory::Blob blob = rex::file::read_file(dummy_file);
  rsl::string_view file_content = rex::memory::blob_to_string_view(blob);
  rsl::vector<rsl::string_view> file_lines = rsl::split(file_content, rex::endline());

  REX_CHECK(file_lines.size() == 1);
  REX_CHECK(file_lines[0] == "this is some dummy content"); 

  rex::file::append_line(dummy_file, dummy_content);

  blob = rex::file::read_file(dummy_file);
  file_content = rex::memory::blob_to_string_view(blob);
  file_lines = rsl::split(file_content, rex::endline());

  REX_CHECK(file_lines.size() == 2);
  REX_CHECK(file_lines[0] == "this is some dummy content");
  REX_CHECK(file_lines[1] == "this is some dummy content");

  rex::file::del(dummy_file);
}

TEST_CASE("TEST - File - Append Lines")
{
  rex::TempCwd tmp_cwd("file_tests");

  rsl::string_view dummy_content = "this is some dummy content";
  rsl::vector<rsl::string> lines;
  lines.emplace_back(dummy_content);

  rex::TempString dummy_file = rex::path::random_filename();
  rex::file::append_lines(dummy_file, lines);

  rex::memory::Blob blob = rex::file::read_file(dummy_file);
  rsl::string_view file_content = rex::memory::blob_to_string_view(blob);
  rsl::vector<rsl::string_view> file_lines = rsl::split(file_content, rex::endline());

  REX_CHECK(file_lines.size() == 1);
  REX_CHECK(file_lines[0] == "this is some dummy content");

  rex::file::append_lines(dummy_file, lines);

  blob = rex::file::read_file(dummy_file);
  file_content = rex::memory::blob_to_string_view(blob);
  file_lines = rsl::split(file_content, rex::endline());

  REX_CHECK(file_lines.size() == 2);
  REX_CHECK(file_lines[0] == "this is some dummy content");
  REX_CHECK(file_lines[1] == "this is some dummy content");

  lines.emplace_back(dummy_content);
  rex::file::append_lines(dummy_file, lines);

  blob = rex::file::read_file(dummy_file);
  file_content = rex::memory::blob_to_string_view(blob);
  file_lines = rsl::split(file_content, rex::endline());

  REX_CHECK(file_lines.size() == 4);
  REX_CHECK(file_lines[0] == "this is some dummy content");
  REX_CHECK(file_lines[1] == "this is some dummy content");
  REX_CHECK(file_lines[2] == "this is some dummy content");
  REX_CHECK(file_lines[3] == "this is some dummy content");

  rex::file::del(dummy_file);
}

TEST_CASE("TEST - File - Append Text")
{
  rex::TempCwd tmp_cwd("file_tests");

  rsl::string_view dummy_content = "this is some dummy content";
  rex::TempString dummy_file = rex::path::random_filename();
  rex::file::append_text(dummy_file, dummy_content);

  rex::memory::Blob blob = rex::file::read_file(dummy_file);
  rsl::string_view file_content = rex::memory::blob_to_string_view(blob);

  REX_CHECK(file_content == "this is some dummy content");

  rex::file::append_text(dummy_file, dummy_content);

  blob = rex::file::read_file(dummy_file);
  file_content = rex::memory::blob_to_string_view(blob);

  REX_CHECK(file_content == "this is some dummy contentthis is some dummy content");

  rex::file::del(dummy_file);
}

TEST_CASE("TEST - File - Truncation")
{
  rex::TempCwd tmp_cwd("file_tests");

  rsl::string_view dummy_content = "this is some dummy content";
  rex::TempString dummy_file = rex::path::random_filename();
  rex::file::append_text(dummy_file, dummy_content);

  rex::memory::Blob blob = rex::file::read_file(dummy_file);
  rsl::string_view file_content = rex::memory::blob_to_string_view(blob);

  REX_CHECK(file_content == "this is some dummy content");

  rex::file::append_text(dummy_file, dummy_content);

  blob = rex::file::read_file(dummy_file);
  file_content = rex::memory::blob_to_string_view(blob);

  REX_CHECK(file_content == "this is some dummy contentthis is some dummy content");

  rex::file::trunc(dummy_file);

  blob = rex::file::read_file(dummy_file);

  REX_CHECK(rex::file::size(dummy_file) == 0_bytes);
  REX_CHECK(blob.size() == 0);

  rex::file::del(dummy_file);
}

TEST_CASE("TEST - File - Copy No Overwrite")
{
  rex::TempCwd tmp_cwd("file_tests");

  const rsl::string_view start_file = "test_file.txt";
  rex::memory::Blob file_content = rex::file::read_file(start_file);
  rsl::string_view file_content_as_string = rex::memory::blob_to_string_view(file_content);

  rex::TempString dummy_file = rex::path::random_filename();

  REX_CHECK(file_content.data() != nullptr);
  REX_CHECK(file_content.size() == 26);
  REX_CHECK(file_content_as_string == "this is some dummy content");

  rex::file::copy(start_file, dummy_file);

  REX_CHECK(rex::file::exists(start_file) == true);
  REX_CHECK(rex::file::exists(dummy_file) == true);

  file_content = rex::file::read_file(dummy_file);
  file_content_as_string = rex::memory::blob_to_string_view(file_content);

  REX_CHECK(file_content.data() != nullptr);
  REX_CHECK(file_content.size() == 26);
  REX_CHECK(file_content_as_string == "this is some dummy content");

  rex::file::del(dummy_file);
}

TEST_CASE("TEST - File - Copy With Overwrite")
{
  rex::TempCwd tmp_cwd("file_tests");

  rsl::string_view start_file = "test_file.txt";;
  rex::memory::Blob file_content = rex::file::read_file(start_file);
  rsl::string_view file_content_as_string = rex::memory::blob_to_string_view(file_content);

  rex::TempString dummy_file = rex::path::random_filename();

  REX_CHECK(file_content.data() != nullptr);
  REX_CHECK(file_content.size() == 26);
  REX_CHECK(file_content_as_string == "this is some dummy content");

  rex::file::create(dummy_file);
  rex::Error err = rex::file::copy(start_file, dummy_file, rex::file::OverwriteIfExist::no);
  REX_CHECK(err.has_error() == true);

  file_content = rex::file::read_file(dummy_file);
  file_content_as_string = rex::memory::blob_to_string_view(file_content);

  REX_CHECK(rex::file::exists(start_file) == true);
  REX_CHECK(rex::file::exists(dummy_file) == true);
  REX_CHECK(file_content.data() == nullptr);
  REX_CHECK(file_content.size() == 0);

  err = rex::file::copy(start_file, dummy_file, rex::file::OverwriteIfExist::yes);
  REX_CHECK(err.has_error() == false);

  REX_CHECK(rex::file::exists(start_file) == true);
  REX_CHECK(rex::file::exists(dummy_file) == true);

  file_content = rex::file::read_file(dummy_file);
  file_content_as_string = rex::memory::blob_to_string_view(file_content);

  REX_CHECK(file_content.data() != nullptr);
  REX_CHECK(file_content.size() == 26);
  REX_CHECK(file_content_as_string == "this is some dummy content");

  rex::file::del(dummy_file);
}

TEST_CASE("TEST - File - Move To Non Existing")
{
  rex::TempCwd tmp_cwd("file_tests");

  const rsl::string_view og_file = "test_file.txt";
  rex::TempString start_file = rex::path::random_filename();
  rex::file::copy(og_file, start_file);
  rex::memory::Blob file_content = rex::file::read_file(start_file);
  rsl::string_view file_content_as_string = rex::memory::blob_to_string_view(file_content);

  rex::TempString dummy_file = rex::path::random_filename();

  REX_CHECK(file_content.data() != nullptr);
  REX_CHECK(file_content.size() == 26);
  REX_CHECK(file_content_as_string == "this is some dummy content");

  rex::file::move(start_file, dummy_file);

  REX_CHECK(rex::file::exists(start_file) == false);
  REX_CHECK(rex::file::exists(dummy_file) == true);

  file_content = rex::file::read_file(dummy_file);
  file_content_as_string = rex::memory::blob_to_string_view(file_content);

  REX_CHECK(file_content.data() != nullptr);
  REX_CHECK(file_content.size() == 26);
  REX_CHECK(file_content_as_string == "this is some dummy content");

  rex::file::del(start_file);
  rex::file::del(dummy_file);
}

TEST_CASE("TEST - File - Move File To Existing")
{
  rex::TempCwd tmp_cwd("file_tests");

  const rsl::string_view og_file = "test_file.txt";
  rex::TempString start_file = rex::path::random_filename();
  rex::file::copy(og_file, start_file);
  rex::memory::Blob file_content = rex::file::read_file(start_file);
  rsl::string_view file_content_as_string = rex::memory::blob_to_string_view(file_content);

  rex::TempString dummy_file = rex::path::random_filename();
  rex::file::create(dummy_file);

  REX_CHECK(file_content.data() != nullptr);
  REX_CHECK(file_content.size() == 26);
  REX_CHECK(file_content_as_string == "this is some dummy content");

  rex::Error err = rex::file::move(start_file, dummy_file);

  REX_CHECK(err.has_error() == true);
  REX_CHECK(rex::file::exists(start_file) == true);
  REX_CHECK(rex::file::exists(dummy_file) == true);

  file_content = rex::file::read_file(dummy_file);
  file_content_as_string = rex::memory::blob_to_string_view(file_content);

  REX_CHECK(file_content.data() == nullptr);
  REX_CHECK(file_content.size() == 0);
  REX_CHECK(file_content_as_string == "");

  rex::file::del(start_file);
  rex::file::del(dummy_file);
}

TEST_CASE("TEST - File - Creation And Deletion")
{
  rex::TempCwd tmp_cwd("file_tests");

  rex::TempString filename = rex::path::random_filename();

  rex::file::create(filename);

  REX_CHECK(rex::file::exists(filename) == true);

  rex::file::del(filename);

  REX_CHECK(rex::file::exists(filename) == false);
}

TEST_CASE("TEST - File - Read Only Checking")
{
  rex::TempCwd tmp_cwd("file_tests");

  rex::TempString filename = rex::path::random_filename();

  rex::file::create(filename);
  REX_CHECK(rex::file::is_readonly(filename) == false);

  rex::file::set_readonly(filename);
  REX_CHECK(rex::file::is_readonly(filename) == true);

  rex::file::remove_readonly(filename);
  REX_CHECK(rex::file::is_readonly(filename) == false);

  rex::file::del(filename);
}

