#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/tracked_allocator.h"
#include "rex_unit_test/test_object.h"

TEST_CASE("TEST - Tracked Allocator - Allocation")
{
	rsl::allocator alloc;
	rex::TrackedAllocator tracked_alloc(alloc);

	void* mem = tracked_alloc.allocate(10);

	REX_CHECK(mem != nullptr);

	delete mem;
}

TEST_CASE("TEST - Tracked Allocator - Construction")
{
	rsl::allocator alloc;
	rex::TrackedAllocator tracked_alloc(alloc);

	rex::test::test_object* mem = static_cast<rex::test::test_object*>(tracked_alloc.allocate(sizeof(rex::test::test_object)));
	tracked_alloc.construct(mem, 1);

	REX_CHECK(mem != nullptr);
	REX_CHECK(mem->x() == 1);

	delete mem;
}