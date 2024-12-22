#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/memory_stats.h"

TEST_CASE("TEST - Memoy Stats - Querying")
{
	rex::MemoryStats stats1 = rex::query_memory_stats();

	REX_CHECK(stats1.total_physical_mem > 0);
	REX_CHECK(stats1.avail_physical_mem > 0); // Can't verify available physical much as it can change by other processes
	REX_CHECK(stats1.total_virtual_mem > 0);
	REX_CHECK(stats1.page_size > 0);
	REX_CHECK(stats1.used_physical_mem > 0);
	REX_CHECK(stats1.used_virtual_mem > 0);

	// an integer is 4 bytes, but the OS might allocate more than that
	// so we can't check for exact memory changes, just that the same or more memory is used
	rex::MemoryStats stats2 = rex::query_memory_stats();

	REX_CHECK(stats2.total_physical_mem == stats1.total_physical_mem);
	REX_CHECK(stats2.total_virtual_mem == stats1.total_virtual_mem);
	REX_CHECK(stats2.page_size == stats1.page_size);
	REX_CHECK(stats2.used_physical_mem >= stats1.used_physical_mem);
	REX_CHECK(stats2.used_virtual_mem >= stats1.used_virtual_mem);

	auto ptr = rsl::make_unique<s32>(1);

	rex::MemoryStats stats3 = rex::query_memory_stats();

	REX_CHECK(stats3.total_physical_mem == stats2.total_physical_mem);
	REX_CHECK(stats3.total_virtual_mem == stats2.total_virtual_mem);
	REX_CHECK(stats3.page_size == stats2.page_size);
	REX_CHECK(stats3.used_physical_mem >= stats2.used_physical_mem);
	REX_CHECK(stats3.used_virtual_mem >= stats2.used_virtual_mem);
}