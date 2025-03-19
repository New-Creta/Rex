#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/threading/thread.h"
#include "rex_engine/engine/types.h"

#include "rex_std/chrono.h"
#include "rex_std/thread.h"

TEST_CASE("TEST - Thread - Construction")
{
	// This should basically not crash nor hold the main thread
	rex::threading::internal::Thread thread;
}

TEST_CASE("TEST - Thread - Run")
{
	s32 x = 0;

	{
		// A thread always joins on destruction
		rex::threading::internal::Thread thread;
		thread.run([&x](void*)
		{
			++x;
			return 0;
		});

		using namespace rsl::chrono_literals;
		rsl::this_thread::sleep_for(1s);
	}

	REX_CHECK(x == 1);
}