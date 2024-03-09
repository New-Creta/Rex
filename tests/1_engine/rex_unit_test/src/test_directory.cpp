#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"

TEST_CASE("Directory Exists")
{
  CHECK(rex::directory::exists("this_is_a_file.txt") == false);
  CHECK(rex::directory::exists("this_is_a_directory"));
  CHECK(rex::directory::exists(rex::path::random_filename()) == false);
  CHECK(rex::directory::exists(rex::path::random_dir()) == false);
}
