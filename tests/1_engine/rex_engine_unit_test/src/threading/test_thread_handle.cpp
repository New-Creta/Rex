#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/threading/thread_handle.h"
#include "rex_engine/engine/types.h"

TEST_CASE("TEST - Thread Handle - Construction")
{
	// This should basically not crash nor hold the main thread
	rex::threading::internal::Thread thread;
	rex::threading::ThreadHandle handle(&thread);

	REX_CHECK(handle.thread() == &thread);

	rex::threading::ThreadHandle handle2(rsl::move(handle));

	REX_CHECK(handle.thread() == nullptr);
	REX_CHECK(handle2.thread() == &thread);
}

TEST_CASE("TEST - Thread Handle - Run")
{
	s32 x = 0;

	{
		// A thread always joins on destruction
		rex::threading::internal::Thread thread;
		rex::threading::ThreadHandle handle(&thread);
		handle.run([&x](void*)
			{
				++x;
				return 0;
			});

		using namespace rsl::chrono_literals;
		rsl::this_thread::sleep_for(1s);
	}

	REX_CHECK(x == 1);
}