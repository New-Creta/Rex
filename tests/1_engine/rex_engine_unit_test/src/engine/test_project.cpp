#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/engine/project.h"

TEST_CASE("Test Project")
{
	REX_CHECK(rex::project_name() == "unknown_project");

	rsl::string_view project_name = "test project";

	rex::set_project_name(project_name);
	REX_CHECK(rex::project_name() == project_name);
}