#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/tmp_cwd.h"
#include "rex_engine/filesystem/tmp_dir.h"
#include "rex_engine/filesystem/tmp_file.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"

TEST_CASE("Test Temp CWD - Default")
{
	rsl::string_view tmp_cwd = "tmp_cwd";
	rsl::string old_cwd = rex::path::cwd();

	{
		rex::TempCwd tmp(tmp_cwd);

		REX_CHECK(rex::path::is_same(tmp_cwd, rex::path::cwd()));
	}

	REX_CHECK(rex::path::is_same(old_cwd, rex::path::cwd()));
}
TEST_CASE("Test Temp CWD - Creating a directory")
{
	rsl::string_view tmp_cwd = "tmp_cwd";
	rsl::string old_cwd = rex::path::cwd();

	rsl::string random_name = rex::path::random_dir();
	{
		rex::TempCwd tmp(tmp_cwd);

		rex::directory::create(random_name);
		REX_CHECK(rex::directory::exists(random_name));
		REX_CHECK(rex::directory::exists(rex::path::join(old_cwd, tmp_cwd, random_name)));
		REX_CHECK(rex::directory::exists(rex::path::join(old_cwd, random_name)) == false);
	}

	REX_CHECK(rex::directory::exists(random_name) == false);
	REX_CHECK(rex::directory::exists(rex::path::join(old_cwd, tmp_cwd, random_name)));
	REX_CHECK(rex::directory::exists(rex::path::join(old_cwd, random_name)) == false);
}
TEST_CASE("Test Temp CWD - Creating a file")
{
	rsl::string_view tmp_cwd = "tmp_cwd";
	rsl::string old_cwd = rex::path::cwd();

	rsl::string random_name = rex::path::random_filename();
	{
		rex::TempCwd tmp(tmp_cwd);

		rex::file::create(random_name);
		REX_CHECK(rex::file::exists(random_name));
		REX_CHECK(rex::file::exists(rex::path::join(old_cwd, tmp_cwd, random_name)));
		REX_CHECK(rex::file::exists(rex::path::join(old_cwd, random_name)) == false);
	}

	REX_CHECK(rex::file::exists(random_name) == false);
	REX_CHECK(rex::file::exists(rex::path::join(old_cwd, tmp_cwd, random_name)));
	REX_CHECK(rex::file::exists(rex::path::join(old_cwd, random_name)) == false);
}