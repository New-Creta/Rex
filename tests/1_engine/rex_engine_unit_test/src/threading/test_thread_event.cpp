#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/threading/thread_event.h"
#include "rex_engine/threading/thread.h"
#include "rex_engine/engine/types.h"

#include "rex_std/chrono.h"
#include "rex_std/thread.h"

TEST_CASE("TEST - Thread Event - Singaling")
{
	s32 x = 0;
	{
		rex::ThreadEvent worker_thread_event;
		rex::ThreadEvent main_thread_event;

		// A thread always joins on destruction
		rex::threading::internal::Thread thread;
		thread.run([&x, &worker_thread_event, &main_thread_event](void*)
			{
				worker_thread_event.wait_for_me();
				worker_thread_event.reset();
				main_thread_event.signal();
				++x;
				main_thread_event.signal();
				return 0;
			});

		// X should not get incremented until the thread even is signaled
		using namespace rsl::chrono_literals;
		rsl::this_thread::sleep_for(1s);
		
		REX_CHECK(x == 0);

		// Signal so the worker thread can continue and increment x
		worker_thread_event.signal();

		// Wait for the x to be increment, and only then continue the main thread
		main_thread_event.wait_for_me();
		main_thread_event.reset();

		REX_CHECK(x == 1);
	}

}