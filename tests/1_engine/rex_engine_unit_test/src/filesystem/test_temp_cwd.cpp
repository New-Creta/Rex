#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/tmp_cwd.h"
#include "rex_engine/filesystem/tmp_dir.h"
#include "rex_engine/filesystem/tmp_file.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"

TEST_CASE("TEST - Temp CWD - Default")
{
	rex::TempDirectory tmp_dir;
	rsl::string old_cwd = rex::path::cwd();

	{
		rex::TempCwd tmp(tmp_dir.dirname());

		REX_CHECK(rex::path::is_same(tmp_dir.dirname(), rex::path::cwd()));
		REX_CHECK(rex::path::is_under_dir(tmp_dir.dirname(), old_cwd));
	}

	REX_CHECK(rex::path::is_same(old_cwd, rex::path::cwd()));
}
TEST_CASE("TEST - Temp CWD - Creating a directory")
{
	rex::TempDirectory tmp_dir;
	rsl::string old_cwd = rex::path::cwd();

	rsl::string random_name = rex::path::random_dir();
	{
		rex::TempCwd tmp(tmp_dir.dirname());

		rex::directory::create(random_name);
		REX_CHECK(rex::directory::exists(random_name));
		REX_CHECK(rex::directory::exists(rex::path::join(tmp_dir.dirname(), random_name)));
	}

	REX_CHECK(rex::directory::exists(random_name) == false);
	REX_CHECK(rex::directory::exists(rex::path::join(tmp_dir.dirname(), random_name)));
}
TEST_CASE("TEST - Temp CWD - Creating a file")
{
	rex::TempDirectory tmp_dir;
	rsl::string old_cwd = rex::path::cwd();

	rsl::string random_name = rex::path::random_filename();
	{
		rex::TempCwd tmp(tmp_dir.dirname());

		rex::file::create(random_name);
		REX_CHECK(rex::file::exists(random_name));
		REX_CHECK(rex::file::exists(rex::path::join(tmp_dir.dirname(), random_name)));
	}

	REX_CHECK(rex::file::exists(random_name) == false);
	REX_CHECK(rex::file::exists(rex::path::join(tmp_dir.dirname(), random_name)));
}