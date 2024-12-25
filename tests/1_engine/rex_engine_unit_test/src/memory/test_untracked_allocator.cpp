#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/untracked_allocator.h"
#include "rex_unit_test/test_object.h"

TEST_CASE("TEST - Tracked Allocator - Allocation")
{
	rsl::allocator alloc;
	rex::UntrackedAllocator untracked_alloc;

	void* mem = untracked_alloc.allocate(10);

	REX_CHECK(mem != nullptr);

	delete mem;
}

TEST_CASE("TEST - Tracked Allocator - Construction")
{
	rsl::allocator alloc;
	rex::UntrackedAllocator untracked_alloc;

	rex::test::test_object* mem = static_cast<rex::test::test_object*>(untracked_alloc.allocate(sizeof(rex::test::test_object)));
	untracked_alloc.construct(mem, 1);

	REX_CHECK(mem != nullptr);
	REX_CHECK(mem->x() == 1);

	delete mem;
}