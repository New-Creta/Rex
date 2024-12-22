#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/debug_allocator.h"
#include "rex_engine/engine/types.h"

TEST_CASE("TEST - Debug Allocator - Allocation And Deallocation")
{
	rsl::allocator alloc;
	rex::DebugAllocator dbg_alloc(alloc);

#ifdef REX_DISABLE_DEBUG_ALLOCATION
	// Allocation with size
	s32 size = 10;
	auto ptr1 = dbg_alloc.allocate(size);
	REX_CHECK(ptr1 == nullptr);
	dbg_alloc.deallocate(ptr1, size);

	// Allocation from type
	auto ptr2 = dbg_alloc.allocate<int>();
	REX_CHECK(ptr2 == nullptr);
	dbg_alloc.deallocate(ptr2);
#else
	// Allocation with size
	s32 size = 10;
	auto ptr1 = dbg_alloc.allocate(size);
	REX_CHECK(ptr1 != nullptr);
	dbg_alloc.deallocate(ptr1, size);

	// Allocation from type
	auto ptr2 = dbg_alloc.allocate<int>();
	REX_CHECK(ptr2 != nullptr);
	dbg_alloc.deallocate(ptr2);
#endif

}