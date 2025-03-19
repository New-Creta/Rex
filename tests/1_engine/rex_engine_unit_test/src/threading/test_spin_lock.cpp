#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/threading/spin_lock.h"
#include "rex_engine/engine/types.h"

TEST_CASE("TEST - Spin Lock - Locking")
{
	rex::SpinLock lock;

	lock.lock();

	for (s32 i = 0; i < 10; ++i)
	{
		// Fails for 10 times
		REX_CHECK(lock.try_lock() == false);
	}

	lock.unlock();
	REX_CHECK(lock.try_lock() == true);
	REX_CHECK(lock.try_lock() == false);
}