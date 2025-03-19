#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/threading/assert_mtx.h"

TEST_CASE("TEST - Assert Mutex - Locking")
{
	rex::AssertMtx mtx{};

	mtx.lock();

	mtx.unlock();

}

TEST_CASE("TEST - Assert Mutex - Try Locking")
{
	rex::AssertMtx mtx{};

	mtx.try_lock();

	mtx.unlock();
}