#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/untracked_allocator.h"
#include "rex_unit_test/test_object.h"

TEST_CASE("TEST - Untracked Allocator - Allocation")
{
	rsl::allocator alloc;
	rex::UntrackedAllocator untracked_alloc;

	void* mem = untracked_alloc.allocate(10);

	REX_CHECK(mem != nullptr);

	untracked_alloc.deallocate(mem, 10);
}

TEST_CASE("TEST - Untracked Allocator - Construction")
{
	rex::test::test_object::reset();

	rsl::allocator alloc;
	rex::UntrackedAllocator untracked_alloc;

	rex::test::test_object* mem = static_cast<rex::test::test_object*>(untracked_alloc.allocate(sizeof(rex::test::test_object)));
	untracked_alloc.construct(mem, 1);

	REX_CHECK(mem != nullptr);
	REX_CHECK(mem->x() == 1);
	REX_CHECK(rex::test::test_object::num_created() == 1);

	untracked_alloc.destroy(mem);
	untracked_alloc.deallocate(mem, sizeof(rex::test::test_object));

	REX_CHECK(rex::test::test_object::num_created() == 1);
	REX_CHECK(rex::test::test_object::num_dtor_calls() == 1);
}