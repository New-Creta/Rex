#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/pooling/scoped_pool_object.h"
#include "rex_engine/pooling/growing_pool.h"
#include "rex_unit_test/test_object.h"

TEST_CASE("TEST - Scoped Pool Object - Construction")
{
	rex::test::test_object::reset();

	rex::ScopedPoolObject<rex::test::test_object> scoped_pool_object;

	REX_CHECK(scoped_pool_object->num_created() == 0);
}

TEST_CASE("TEST - Scoped Pool Object - Auto Return To Pool")
{
	rex::test::test_object::reset();
	rex::GrowingPool<rex::test::test_object, s32> growing_pool;

	s32 x = 0;

	{
		rex::test::test_object* ptr = growing_pool.request([](const rsl::unique_ptr<rex::test::test_object>& ptr) { return 1; }, 3);
		rex::ScopedPoolObject<rex::test::test_object> scoped_obj(ptr, [&x](rex::test::test_object* ptr) { ++x; });

		REX_CHECK(scoped_obj->x() == 3);
		REX_CHECK(growing_pool.num_active_objects() == 1);
		REX_CHECK(growing_pool.num_idle_objects() == 0);
		REX_CHECK(growing_pool.max_active_objects() == 1);
		REX_CHECK(growing_pool.max_idle_objects() == 1);
		REX_CHECK(rex::test::test_object::num_created() == 1);
		REX_CHECK(x == 1);
	}

	REX_CHECK(growing_pool.num_active_objects() == 0);
	REX_CHECK(growing_pool.num_idle_objects() == 1);
	REX_CHECK(growing_pool.max_active_objects() == 1);
	REX_CHECK(growing_pool.max_idle_objects() == 1);
	REX_CHECK(rex::test::test_object::num_created() == 1);
	REX_CHECK(rex::test::test_object::num_dtor_calls() == 1);
	REX_CHECK(x == 2);
}