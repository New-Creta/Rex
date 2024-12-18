#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/engine/invalid_object.h"

TEST_CASE("Invalid object")
{
	int x = rex::invalid_obj<int>();

	REX_CHECK(x == rex::s_invalid_obj_value);
}