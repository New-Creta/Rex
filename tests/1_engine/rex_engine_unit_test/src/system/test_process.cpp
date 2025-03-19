#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/system/process.h"

TEST_CASE("TEST - Process - Process ID")
{
	REX_CHECK(rex::current_process_id() != 0);
}