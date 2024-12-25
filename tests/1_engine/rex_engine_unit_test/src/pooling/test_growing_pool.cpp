#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/pooling/growing_pool.h"

TEST_CASE("TEST - Growing Pool - Construction")
{
	rex::GrowingPool<s32> growing_pool([]() { return rsl::make_unique<s32>(1); });

	REX_CHECK(growing_pool.num_active_objects() == 0);
	REX_CHECK(growing_pool.num_idle_objects() == 0);
}

TEST_CASE("TEST - Growing Pool - Request a single item")
{

}