#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/pooling/growing_pool.h"
#include "rex_unit_test/test_object.h"

TEST_CASE("TEST - Growing Pool - Construction")
{
	rex::test::test_object::reset();

	rex::GrowingPool<rex::test::test_object> growing_pool;

	REX_CHECK(growing_pool.num_active_objects() == 0);
	REX_CHECK(growing_pool.num_idle_objects() == 0);
	REX_CHECK(growing_pool.max_active_objects() == 0);
	REX_CHECK(growing_pool.max_idle_objects() == 0);
	REX_CHECK(rex::test::test_object::num_created() == 0);
}

TEST_CASE("TEST - Growing Pool - Request a single item")
{
	rex::test::test_object::reset();

	rex::GrowingPool<rex::test::test_object, s32> growing_pool;

	rex::test::test_object* ptr = growing_pool.request([](const rsl::unique_ptr<rex::test::test_object>& /*ptr*/) { return true; }, 2);

	REX_CHECK(ptr->x() == 2);
	REX_CHECK(growing_pool.num_active_objects() == 1);
	REX_CHECK(growing_pool.num_idle_objects() == 0);
	REX_CHECK(growing_pool.max_active_objects() >= 1);
	REX_CHECK(growing_pool.max_idle_objects() >= 1);
	REX_CHECK(rex::test::test_object::num_created() == 1);

	growing_pool.discard(ptr);

	REX_CHECK(growing_pool.num_active_objects() == 0);
	REX_CHECK(growing_pool.num_idle_objects() == 1);
	REX_CHECK(growing_pool.max_active_objects() >= 1);
	REX_CHECK(growing_pool.max_idle_objects() >= 1);
	REX_CHECK(rex::test::test_object::num_created() == 1);
	REX_CHECK(rex::test::test_object::num_dtor_calls() == 0); // No dtor is called when an object is made idle
}

TEST_CASE("Test - Growing Pool - Request Scoped Item")
{
	rex::GrowingPool<rex::test::test_object, s32> growing_pool;

	rex::test::test_object::reset();

	{
		rex::ObjectWithDestructionCallback<rex::test::test_object> scoped_obj = growing_pool.request_scoped([](const rsl::unique_ptr<rex::test::test_object>& /*ptr*/) { return true; }, 2);

		REX_CHECK(scoped_obj->x() == 2);
		REX_CHECK(growing_pool.num_active_objects() == 1);
		REX_CHECK(growing_pool.num_idle_objects() == 0);
		REX_CHECK(growing_pool.max_active_objects() >= 1);
		REX_CHECK(growing_pool.max_idle_objects() >= 1);
		REX_CHECK(rex::test::test_object::num_created() == 1);
	}

	REX_CHECK(growing_pool.num_active_objects() == 0);
	REX_CHECK(growing_pool.num_idle_objects() == 1);
	REX_CHECK(growing_pool.max_active_objects() >= 1);
	REX_CHECK(growing_pool.max_idle_objects() >= 1);
	REX_CHECK(rex::test::test_object::num_created() == 1);
	REX_CHECK(rex::test::test_object::num_dtor_calls() == 0);

}

TEST_CASE("TEST - Growling Pool - Request multiple items")
{
	rex::test::test_object::reset();

	rex::GrowingPool<rex::test::test_object, s32> growing_pool;

	rex::test::test_object* ptr1 = growing_pool.request([](const rsl::unique_ptr<rex::test::test_object>& /*ptr*/) { return true; }, 2);
	rex::test::test_object* ptr2 = growing_pool.request([](const rsl::unique_ptr<rex::test::test_object>& /*ptr*/) { return true; }, 3);

	REX_CHECK(ptr1->x() == 2);
	REX_CHECK(ptr2->x() == 3);
	REX_CHECK(growing_pool.num_active_objects() == 2);
	REX_CHECK(growing_pool.num_idle_objects() == 0);
	REX_CHECK(growing_pool.max_active_objects() >= 2);
	REX_CHECK(growing_pool.max_idle_objects() >= 2);
	REX_CHECK(rex::test::test_object::num_created() == 2);
	REX_CHECK(rex::test::test_object::num_dtor_calls() == 0);

	growing_pool.discard(ptr1);
	REX_CHECK(growing_pool.num_active_objects() == 1);
	REX_CHECK(growing_pool.num_idle_objects() == 1);
	REX_CHECK(growing_pool.max_active_objects() >= 2);
	REX_CHECK(growing_pool.max_idle_objects() >= 2);
	REX_CHECK(rex::test::test_object::num_created() == 2);
	REX_CHECK(rex::test::test_object::num_dtor_calls() == 0);

	growing_pool.discard(ptr2);
	REX_CHECK(growing_pool.num_active_objects() == 0);
	REX_CHECK(growing_pool.num_idle_objects() == 2);
	REX_CHECK(growing_pool.max_active_objects() >= 2);
	REX_CHECK(growing_pool.max_idle_objects() >= 2);
	REX_CHECK(rex::test::test_object::num_created() == 2);
	REX_CHECK(rex::test::test_object::num_dtor_calls() == 0);
}

TEST_CASE("TEST - Growling Pool - Resizing")
{
	rex::test::test_object::reset();

	rex::GrowingPool<rex::test::test_object> growing_pool;

	growing_pool.resize(3);

	REX_CHECK(growing_pool.num_active_objects() == 0);
	REX_CHECK(growing_pool.num_idle_objects() == 3);
	REX_CHECK(growing_pool.max_active_objects() == 3);
	REX_CHECK(growing_pool.max_idle_objects() == 3);
	REX_CHECK(rex::test::test_object::num_created() == 3);
	REX_CHECK(rex::test::test_object::num_dtor_calls() == 0);

}