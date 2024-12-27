#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/shapes/rect.h"

TEST_CASE("TEST - Shapes - Rect")
{
	rex::Rect8 rect{};

	REX_CHECK(rect.top_left.x == 0);
	REX_CHECK(rect.top_left.y == 0);
	REX_CHECK(rect.bottom_right.x == 0);
	REX_CHECK(rect.bottom_right.y == 0);
}