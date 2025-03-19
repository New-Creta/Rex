#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/memory_stats.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/memory/memory_header.h"

TEST_CASE("TEST - Memory Stats - System Stats")
{
	rex::SystemMemoryStats stats = rex::query_system_mem_stats();

	REX_CHECK(stats.avail_physical_mem > 0);
	REX_CHECK(stats.avail_virtual_mem > 0);
	REX_CHECK(stats.page_size > 0);
	REX_CHECK(stats.total_physical_mem > 0);
	REX_CHECK(stats.total_virtual_mem > 0);
}

TEST_CASE("TEST - Memoy Stats - Process Stats")
{
	rex::ProcessMemoryStats stats = rex::query_process_mem_stats();

	REX_CHECK(stats.used_physical_mem > 0);
	REX_CHECK(stats.peak_physical_mem > 0);
	REX_CHECK(stats.used_virtual_mem > 0);
	REX_CHECK(stats.peak_virtual_mem > 0);
	REX_CHECK(stats.num_page_faults >= 0);
}

TEST_CASE("TEST - Memory Stats - Memory Tracking Stats")
{
	rex::MemoryTrackingStats stats1 = rex::query_mem_tracking_stats();

	REX_CHECK(stats1.max_used_memory > 0);
	REX_CHECK(stats1.max_used_memory >= stats1.used_memory);
	REX_CHECK(stats1.used_memory > 0);
	REX_CHECK(stats1.num_alive_allocations > 0);
	REX_CHECK(stats1.num_total_allocations > 0);
	REX_CHECK(stats1.num_total_allocations >= stats1.num_alive_allocations);
	REX_CHECK(stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value() > 0);
	REX_CHECK(stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value() > 0);
	REX_CHECK(stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value() == 0);
	REX_CHECK(stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value() == 0);

	// Stack allocation of a single int, should not affect tracked memory allocation
	rex::MemoryTrackingStats stats2 = rex::query_mem_tracking_stats();

	REX_CHECK(stats2.max_used_memory == stats1.max_used_memory);
	REX_CHECK(stats2.used_memory == stats1.used_memory);
	REX_CHECK(stats2.num_alive_allocations == stats1.num_alive_allocations);
	REX_CHECK(stats2.num_total_allocations == stats1.num_total_allocations);
	REX_CHECK(stats2.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value());
	REX_CHECK(stats2.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value());
	REX_CHECK(stats2.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value());
	REX_CHECK(stats2.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value());

	// Heap allocation of a single int, should increase the tracked allocations by 1
	s32* y = new int(3);
	s32 debug_allocation_overhead = 0;
#ifdef REX_ENABLE_MEM_TRACKING
	debug_allocation_overhead += sizeof(rex::InlineMemoryHeader);
#endif
	rex::MemoryTrackingStats stats3 = rex::query_mem_tracking_stats();
	REX_CHECK(stats3.max_used_memory >= stats1.max_used_memory);
	// Tracking memory also costs memory, so we can't guarantee that a single allocation results in X memory added
	// However, we can guarantee that it'll be bigger
	REX_CHECK(stats3.used_memory >= stats1.used_memory.size_in_bytes() + static_cast<s32>(sizeof(s32)) + debug_allocation_overhead);
	REX_CHECK(stats3.num_alive_allocations == stats1.num_alive_allocations + 1);
	REX_CHECK(stats3.num_total_allocations == stats1.num_total_allocations + 1);
	REX_CHECK(stats3.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value() + static_cast<s32>(sizeof(s32)) + debug_allocation_overhead);
	REX_CHECK(stats3.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value() >= stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value());
	REX_CHECK(stats3.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value());
	REX_CHECK(stats3.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value());
	delete y;

	// After the deallocation, the memory should come back
	rex::MemoryTrackingStats stats4 = rex::query_mem_tracking_stats();
	REX_CHECK(stats4.max_used_memory >= stats1.max_used_memory);
	REX_CHECK(stats4.used_memory == stats1.used_memory);
	REX_CHECK(stats4.num_alive_allocations == stats1.num_alive_allocations);
	REX_CHECK(stats4.num_total_allocations == stats1.num_total_allocations + 1);
	REX_CHECK(stats4.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value());
	REX_CHECK(stats4.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value());
	REX_CHECK(stats4.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value());
	REX_CHECK(stats4.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value() == stats1.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value());

}

TEST_CASE("TEST - Memory Stats - All Memory Stats")
{
	// This is essentially the same as the above, just put together into 1 test case
	rex::MemoryStats stats1 = rex::query_all_memory_stats();

	// System Memory Stats

	REX_CHECK(stats1.system_mem_stats.avail_physical_mem > 0);
	REX_CHECK(stats1.system_mem_stats.avail_virtual_mem > 0);
	REX_CHECK(stats1.system_mem_stats.page_size > 0);
	REX_CHECK(stats1.system_mem_stats.total_physical_mem > 0);
	REX_CHECK(stats1.system_mem_stats.total_virtual_mem > 0);

	// Process Memory Stats

	REX_CHECK(stats1.process_mem_stats.used_physical_mem > 0);
	REX_CHECK(stats1.process_mem_stats.peak_physical_mem > 0);
	REX_CHECK(stats1.process_mem_stats.used_virtual_mem > 0);
	REX_CHECK(stats1.process_mem_stats.peak_virtual_mem > 0);
	REX_CHECK(stats1.process_mem_stats.num_page_faults >= 0);

	// Memory tracking

	REX_CHECK(stats1.mem_tracking_stats.max_used_memory > 0);
	REX_CHECK(stats1.mem_tracking_stats.max_used_memory >= stats1.mem_tracking_stats.used_memory);
	REX_CHECK(stats1.mem_tracking_stats.used_memory > 0);
	REX_CHECK(stats1.mem_tracking_stats.num_alive_allocations > 0);
	REX_CHECK(stats1.mem_tracking_stats.num_total_allocations > 0);
	REX_CHECK(stats1.mem_tracking_stats.num_total_allocations >= stats1.mem_tracking_stats.num_alive_allocations);
	REX_CHECK(stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value() > 0);
	REX_CHECK(stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value() > 0);
	REX_CHECK(stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value() == 0);
	REX_CHECK(stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value() == 0);

	// Stack allocation of a single int, should not affect tracked memory allocation
	rex::MemoryStats stats2 = rex::query_all_memory_stats();

	// System Memory Stats

	REX_CHECK(stats2.system_mem_stats.avail_physical_mem > 0);
	REX_CHECK(stats2.system_mem_stats.avail_virtual_mem > 0);
	REX_CHECK(stats2.system_mem_stats.page_size > 0);
	REX_CHECK(stats2.system_mem_stats.total_physical_mem > 0);
	REX_CHECK(stats2.system_mem_stats.total_virtual_mem > 0);

	// Process Memory Stats

	REX_CHECK(stats2.process_mem_stats.used_physical_mem > 0);
	REX_CHECK(stats2.process_mem_stats.peak_physical_mem > 0);
	REX_CHECK(stats2.process_mem_stats.used_virtual_mem > 0);
	REX_CHECK(stats2.process_mem_stats.peak_virtual_mem > 0);
	REX_CHECK(stats2.process_mem_stats.num_page_faults >= 0);

	REX_CHECK(stats2.mem_tracking_stats.max_used_memory == stats1.mem_tracking_stats.max_used_memory);
	REX_CHECK(stats2.mem_tracking_stats.used_memory == stats1.mem_tracking_stats.used_memory);
	REX_CHECK(stats2.mem_tracking_stats.num_alive_allocations == stats1.mem_tracking_stats.num_alive_allocations);
	REX_CHECK(stats2.mem_tracking_stats.num_total_allocations == stats1.mem_tracking_stats.num_total_allocations);
	REX_CHECK(stats2.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value());
	REX_CHECK(stats2.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value());
	REX_CHECK(stats2.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value());
	REX_CHECK(stats2.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value());

	// Heap allocation of a single int, should increase the tracked allocations by 1
	s32* y = new int(3);
	s32 debug_allocation_overhead = 0;
#ifdef REX_ENABLE_MEM_TRACKING
	debug_allocation_overhead += sizeof(rex::InlineMemoryHeader);
#endif
	rex::MemoryStats stats3 = rex::query_all_memory_stats();

	// System Memory Stats

	REX_CHECK(stats2.system_mem_stats.avail_physical_mem > 0);
	REX_CHECK(stats2.system_mem_stats.avail_virtual_mem > 0);
	REX_CHECK(stats2.system_mem_stats.page_size > 0);
	REX_CHECK(stats2.system_mem_stats.total_physical_mem > 0);
	REX_CHECK(stats2.system_mem_stats.total_virtual_mem > 0);

	// Process Memory Stats

	REX_CHECK(stats2.process_mem_stats.used_physical_mem > 0);
	REX_CHECK(stats2.process_mem_stats.peak_physical_mem > 0);
	REX_CHECK(stats2.process_mem_stats.used_virtual_mem > 0);
	REX_CHECK(stats2.process_mem_stats.peak_virtual_mem > 0);
	REX_CHECK(stats2.process_mem_stats.num_page_faults >= 0);

	REX_CHECK(stats3.mem_tracking_stats.max_used_memory >= stats1.mem_tracking_stats.max_used_memory);
	// Tracking memory also costs memory, so we can't guarantee that a single allocation results in X memory added
	// However, we can guarantee that it'll be bigger
	REX_CHECK(stats3.mem_tracking_stats.used_memory >= stats1.mem_tracking_stats.used_memory.size_in_bytes() + static_cast<s32>(sizeof(s32)) + debug_allocation_overhead);
	REX_CHECK(stats3.mem_tracking_stats.num_alive_allocations == stats1.mem_tracking_stats.num_alive_allocations + 1);
	REX_CHECK(stats3.mem_tracking_stats.num_total_allocations == stats1.mem_tracking_stats.num_total_allocations + 1);
	REX_CHECK(stats3.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value() + static_cast<s32>(sizeof(s32)) + debug_allocation_overhead);
	REX_CHECK(stats3.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value() >= stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value());
	REX_CHECK(stats3.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value());
	REX_CHECK(stats3.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value());
	delete y;

	// After the deallocation, the memory should come back
	rex::MemoryStats stats4 = rex::query_all_memory_stats();

	// System Memory Stats

	REX_CHECK(stats2.system_mem_stats.avail_physical_mem > 0);
	REX_CHECK(stats2.system_mem_stats.avail_virtual_mem > 0);
	REX_CHECK(stats2.system_mem_stats.page_size > 0);
	REX_CHECK(stats2.system_mem_stats.total_physical_mem > 0);
	REX_CHECK(stats2.system_mem_stats.total_virtual_mem > 0);

	// Process Memory Stats

	REX_CHECK(stats2.process_mem_stats.used_physical_mem > 0);
	REX_CHECK(stats2.process_mem_stats.peak_physical_mem > 0);
	REX_CHECK(stats2.process_mem_stats.used_virtual_mem > 0);
	REX_CHECK(stats2.process_mem_stats.peak_virtual_mem > 0);
	REX_CHECK(stats2.process_mem_stats.num_page_faults >= 0);

	REX_CHECK(stats4.mem_tracking_stats.max_used_memory >= stats1.mem_tracking_stats.max_used_memory);
	REX_CHECK(stats4.mem_tracking_stats.used_memory == stats1.mem_tracking_stats.used_memory);
	REX_CHECK(stats4.mem_tracking_stats.num_alive_allocations == stats1.mem_tracking_stats.num_alive_allocations);
	REX_CHECK(stats4.mem_tracking_stats.num_total_allocations == stats1.mem_tracking_stats.num_total_allocations + 1);
	REX_CHECK(stats4.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].value());
	REX_CHECK(stats4.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Global).value()].max_value());
	REX_CHECK(stats4.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::Engine).value()].value());
	REX_CHECK(stats4.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value() == stats1.mem_tracking_stats.usage_per_tag[rsl::enum_refl::enum_index(rex::MemoryTag::FileIO).value()].value());
}