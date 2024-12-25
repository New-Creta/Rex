#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/memory/memory_header.h"
#include "rex_engine/engine/casting.h"

TEST_CASE("TEST - Memory Tracking - Single Memory Tag Allocation")
{
	rex::MemoryTrackingStats stats = rex::mem_tracker().current_tracking_stats();
	s32 current_global_mem_usage = stats.usage_per_tag[rex::to_int(rex::MemoryTag::Global)].value();
	s32 debug_allocation_overhead = 0;
#ifdef REX_ENABLE_MEM_TRACKING
	debug_allocation_overhead += sizeof(rex::InlineMemoryHeader);
#endif

	// memory usage after a stack allocation should still be the same
	// I know the compiler tends to preallocate the stack storage
	// but either way, I'm testing for it
	s32 x = 0;
	stats = rex::mem_tracker().current_tracking_stats();
	REX_CHECK(current_global_mem_usage == stats.usage_per_tag[rex::to_int(rex::MemoryTag::Global)].value());

	s32* y = new int(1);
	stats = rex::mem_tracker().current_tracking_stats();
	REX_CHECK(current_global_mem_usage + sizeof(s32) + debug_allocation_overhead == stats.usage_per_tag[rex::to_int(rex::MemoryTag::Global)].value());

	delete y;
	stats = rex::mem_tracker().current_tracking_stats();
	REX_CHECK(current_global_mem_usage == stats.usage_per_tag[rex::to_int(rex::MemoryTag::Global)].value());
}

TEST_CASE("TEST - Memory Tracking - Multiple Memory Tag Allocation")
{
	rex::MemoryTrackingStats stats = rex::mem_tracker().current_tracking_stats();
	s32 current_global_mem_usage = stats.usage_per_tag[rex::to_int(rex::MemoryTag::Global)].value();
	s32 current_engine_mem_usage = stats.usage_per_tag[rex::to_int(rex::MemoryTag::Engine)].value();
	s32 debug_allocation_overhead = 0;
#ifdef REX_ENABLE_MEM_TRACKING
	debug_allocation_overhead += sizeof(rex::InlineMemoryHeader);
#endif

	{
		REX_MEM_TAG_SCOPE(rex::MemoryTag::Engine);
		s32* y = new int(1);

		stats = rex::mem_tracker().current_tracking_stats();
		REX_CHECK(current_global_mem_usage == stats.usage_per_tag[rex::to_int(rex::MemoryTag::Global)].value());
		REX_CHECK(current_engine_mem_usage + sizeof(s32) + debug_allocation_overhead == stats.usage_per_tag[rex::to_int(rex::MemoryTag::Engine)].value());

		delete y;
	}

	stats = rex::mem_tracker().current_tracking_stats();
	REX_CHECK(current_global_mem_usage == stats.usage_per_tag[rex::to_int(rex::MemoryTag::Global)].value());
	REX_CHECK(current_engine_mem_usage == stats.usage_per_tag[rex::to_int(rex::MemoryTag::Engine)].value());
}