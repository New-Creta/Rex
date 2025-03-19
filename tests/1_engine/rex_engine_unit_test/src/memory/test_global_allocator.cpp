#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/global_allocator.h"

#include "rex_unit_test/test_object.h"

TEST_CASE("TEST - Global Allocator - Normal Allocation")
{
	s32 size = 10;
	void* ptr1 = rex::alloc(size);
	REX_CHECK(ptr1 != nullptr);
	rex::dealloc(ptr1, size);

	using namespace rex::test;

	test_object::reset();

	REX_CHECK(test_object::num_created() == 0);
	REX_CHECK(test_object::num_ctor_calls() == 0);
	REX_CHECK(test_object::num_copy_ctor_calls() == 0);
	REX_CHECK(test_object::num_move_ctor_calls() == 0);
	REX_CHECK(test_object::num_dtor_calls() == 0);
	REX_CHECK(test_object::num_copy_assignment_calls() == 0);
	REX_CHECK(test_object::num_move_assignment_calls() == 0);

	test_object* ptr2 = rex::alloc<test_object>();

	REX_CHECK(ptr2 != nullptr);
	REX_CHECK(test_object::num_created() == 1);
	REX_CHECK(test_object::num_ctor_calls() == 1);
	REX_CHECK(test_object::num_copy_ctor_calls() == 0);
	REX_CHECK(test_object::num_move_ctor_calls() == 0);
	REX_CHECK(test_object::num_dtor_calls() == 0);
	REX_CHECK(test_object::num_copy_assignment_calls() == 0);
	REX_CHECK(test_object::num_move_assignment_calls() == 0);

	rex::dealloc(ptr2);

	REX_CHECK(test_object::num_created() == 1);
	REX_CHECK(test_object::num_ctor_calls() == 1);
	REX_CHECK(test_object::num_copy_ctor_calls() == 0);
	REX_CHECK(test_object::num_move_ctor_calls() == 0);
	REX_CHECK(test_object::num_dtor_calls() == 1);
	REX_CHECK(test_object::num_copy_assignment_calls() == 0);
	REX_CHECK(test_object::num_move_assignment_calls() == 0);
}
TEST_CASE("TEST - Global Allocator - Debug Allocation")
{
#ifdef REX_DISABLE_DEBUG_ALLOCATION
	// Allocation with size
	s32 size = 10;
	auto ptr1 = rex::debug_alloc(size);
	REX_CHECK(ptr1 == nullptr);
	rex::debug_dealloc(ptr1, size);

	// Allocation from type
	using namespace rex::test;

	test_object::reset();

	REX_CHECK(test_object::num_created() == 0);
	REX_CHECK(test_object::num_ctor_calls() == 0);
	REX_CHECK(test_object::num_copy_ctor_calls() == 0);
	REX_CHECK(test_object::num_move_ctor_calls() == 0);
	REX_CHECK(test_object::num_dtor_calls() == 0);
	REX_CHECK(test_object::num_copy_assignment_calls() == 0);
	REX_CHECK(test_object::num_move_assignment_calls() == 0);

	test_object* ptr2 = rex::alloc<test_object>();

	REX_CHECK(ptr2 == nullptr);
	REX_CHECK(test_object::num_created() == 0);
	REX_CHECK(test_object::num_ctor_calls() == 0);
	REX_CHECK(test_object::num_copy_ctor_calls() == 0);
	REX_CHECK(test_object::num_move_ctor_calls() == 0);
	REX_CHECK(test_object::num_dtor_calls() == 0);
	REX_CHECK(test_object::num_copy_assignment_calls() == 0);
	REX_CHECK(test_object::num_move_assignment_calls() == 0);

	rex::dealloc(ptr2);

	REX_CHECK(test_object::num_created() == 0);
	REX_CHECK(test_object::num_ctor_calls() == 0);
	REX_CHECK(test_object::num_copy_ctor_calls() == 0);
	REX_CHECK(test_object::num_move_ctor_calls() == 0);
	REX_CHECK(test_object::num_dtor_calls() == 0);
	REX_CHECK(test_object::num_copy_assignment_calls() == 0);
	REX_CHECK(test_object::num_move_assignment_calls() == 0);
#else
	// Allocation with size
	s32 size = 10;
	auto ptr1 = rex::debug_alloc(size);
	REX_CHECK(ptr1 != nullptr);
	rex::debug_dealloc(ptr1, size);

	using namespace rex::test;

	test_object::reset();

	REX_CHECK(test_object::num_created() == 0);
	REX_CHECK(test_object::num_ctor_calls() == 0);
	REX_CHECK(test_object::num_copy_ctor_calls() == 0);
	REX_CHECK(test_object::num_move_ctor_calls() == 0);
	REX_CHECK(test_object::num_dtor_calls() == 0);
	REX_CHECK(test_object::num_copy_assignment_calls() == 0);
	REX_CHECK(test_object::num_move_assignment_calls() == 0);

	test_object* ptr2 = rex::debug_alloc<test_object>();

	REX_CHECK(ptr2 != nullptr);
	REX_CHECK(test_object::num_created() == 1);
	REX_CHECK(test_object::num_ctor_calls() == 1);
	REX_CHECK(test_object::num_copy_ctor_calls() == 0);
	REX_CHECK(test_object::num_move_ctor_calls() == 0);
	REX_CHECK(test_object::num_dtor_calls() == 0);
	REX_CHECK(test_object::num_copy_assignment_calls() == 0);
	REX_CHECK(test_object::num_move_assignment_calls() == 0);

	rex::debug_dealloc(ptr2);

	REX_CHECK(test_object::num_created() == 1);
	REX_CHECK(test_object::num_ctor_calls() == 1);
	REX_CHECK(test_object::num_copy_ctor_calls() == 0);
	REX_CHECK(test_object::num_move_ctor_calls() == 0);
	REX_CHECK(test_object::num_dtor_calls() == 1);
	REX_CHECK(test_object::num_copy_assignment_calls() == 0);
	REX_CHECK(test_object::num_move_assignment_calls() == 0); 
#endif
}
TEST_CASE("Test - Global Allocator - Debug Unique Ptr")
{
	using namespace rex::test;

	// default ctor
	{
		test_object::reset();

		rex::unique_debug_ptr<test_object> ptr;

		REX_CHECK(ptr.get() == nullptr);
		REX_CHECK(ptr.operator->() == nullptr);
		REX_CHECK(ptr.operator bool() == false);
		REX_CHECK(test_object::num_created() == 0);
		REX_CHECK(test_object::num_ctor_calls() == 0);
		REX_CHECK(test_object::num_copy_ctor_calls() == 0);
		REX_CHECK(test_object::num_move_ctor_calls() == 0);
		REX_CHECK(test_object::num_dtor_calls() == 0);
		REX_CHECK(test_object::num_copy_assignment_calls() == 0);
		REX_CHECK(test_object::num_move_assignment_calls() == 0);
	}

	REX_CHECK(test_object::num_created() == 0);
	REX_CHECK(test_object::num_ctor_calls() == 0);
	REX_CHECK(test_object::num_copy_ctor_calls() == 0);
	REX_CHECK(test_object::num_move_ctor_calls() == 0);
	REX_CHECK(test_object::num_dtor_calls() == 0);
	REX_CHECK(test_object::num_copy_assignment_calls() == 0);
	REX_CHECK(test_object::num_move_assignment_calls() == 0);

	// constructor call using make_unique_debug
#ifdef REX_DISABLE_DEBUG_ALLOCATION
	{
		test_object::reset();

		rex::unique_debug_ptr<test_object> ptr = rex::make_unique_debug<test_object>(1);

		CHECK(ptr.get() == nullptr);
		CHECK(ptr.operator->() != nullptr);
		CHECK(ptr.operator bool() == true);
		CHECK(test_object::num_created() == 0);
		CHECK(test_object::num_ctor_calls() == 0);
		CHECK(test_object::num_copy_ctor_calls() == 0);
		CHECK(test_object::num_move_ctor_calls() == 0);
		CHECK(test_object::num_dtor_calls() == 0);
		CHECK(test_object::num_copy_assignment_calls() == 0);
		CHECK(test_object::num_move_assignment_calls() == 0);
	}

	CHECK(test_object::num_created() == 0);
	CHECK(test_object::num_ctor_calls() == 0);
	CHECK(test_object::num_copy_ctor_calls() == 0);
	CHECK(test_object::num_move_ctor_calls() == 0);
	CHECK(test_object::num_dtor_calls() == 0);
	CHECK(test_object::num_copy_assignment_calls() == 0);
	CHECK(test_object::num_move_assignment_calls() == 0);
#else
	{
		test_object::reset();

		rex::unique_debug_ptr<test_object> ptr = rex::make_unique_debug<test_object>(1);

		CHECK(ptr.get() != nullptr);
		CHECK(ptr.operator->() != nullptr);
		CHECK(ptr.operator bool() == true);
		CHECK(*ptr == 1);
		CHECK(*ptr.get() == 1);
		CHECK(test_object::num_created() == 1);
		CHECK(test_object::num_ctor_calls() == 1);
		CHECK(test_object::num_copy_ctor_calls() == 0);
		CHECK(test_object::num_move_ctor_calls() == 0);
		CHECK(test_object::num_dtor_calls() == 0);
		CHECK(test_object::num_copy_assignment_calls() == 0);
		CHECK(test_object::num_move_assignment_calls() == 0);
	}

	CHECK(test_object::num_created() == 1);
	CHECK(test_object::num_ctor_calls() == 1);
	CHECK(test_object::num_copy_ctor_calls() == 0);
	CHECK(test_object::num_move_ctor_calls() == 0);
	CHECK(test_object::num_dtor_calls() == 1);
	CHECK(test_object::num_copy_assignment_calls() == 0);
	CHECK(test_object::num_move_assignment_calls() == 0);
#endif
}