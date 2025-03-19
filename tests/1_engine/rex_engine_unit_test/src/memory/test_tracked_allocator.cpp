#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/tracked_allocator.h"
#include "rex_unit_test/test_object.h"

TEST_CASE("TEST - Tracked Allocator - Allocation")
{
	rsl::allocator alloc;
	rex::TrackedAllocator tracked_alloc(alloc);

	void* mem = tracked_alloc.allocate(10);

	REX_CHECK(mem != nullptr);

	tracked_alloc.deallocate(mem, 10);
}

TEST_CASE("TEST - Tracked Allocator - Construction")
{
	rex::test::test_object::reset();

	rex::test::test_object::reset();

	rsl::allocator alloc;
	rex::TrackedAllocator tracked_alloc(alloc);

	rex::test::test_object* mem = static_cast<rex::test::test_object*>(tracked_alloc.allocate(sizeof(rex::test::test_object)));
	tracked_alloc.construct(mem, 1);

	REX_CHECK(mem != nullptr);
	REX_CHECK(mem->x() == 1);
	REX_CHECK(rex::test::test_object::num_created() == 1);

	tracked_alloc.destroy(mem);
	tracked_alloc.deallocate(mem, sizeof(rex::test::test_object));

	REX_CHECK(rex::test::test_object::num_created() == 1);
	REX_CHECK(rex::test::test_object::num_dtor_calls() == 1);

}