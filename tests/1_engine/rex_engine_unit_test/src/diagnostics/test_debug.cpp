#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/diagnostics/debug.h"

TEST_CASE("Test if debugger is attached")
{
	if (!rex::is_debugger_attached())
	{
		rex::attach_debugger();
	}

	REX_CHECK(rex::is_debugger_attached() == true);
}