#include "regina/content_manager.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"

#include "rex_engine/memory/memory_tracking.h"

namespace regina
{
	DEFINE_LOG_CATEGORY(LogContentManager);

	ContentManager::ContentManager()
	{
		REX_INFO(LogContentManager, "Scanning data directory \"{}\"", rex::path::cwd());
	}

	void ContentManager::add_content(rsl::string_view directory)
	{
		REX_INFO(LogContentManager, "Adding directory {}", directory);

		auto current_tracking_stats = rex::mem_tracker().current_tracking_stats();

		rsl::vector<rsl::string> entries = rex::directory::list_entries(directory, rex::directory::Recursive::yes);

		auto new_tracking_stats = rex::mem_tracker().current_tracking_stats();

		REX_INFO(LogContentManager, "Content manager caused {} allocations", new_tracking_stats.num_total_allocations - current_tracking_stats.num_total_allocations);

		for (rsl::string_view entry : entries)
		{
 			if (rex::directory::exists(entry))
			{
				REX_INFO(LogContentManager, "Found directory: {}", entry);
			}
			else
			{
				REX_INFO(LogContentManager, "Found file: {}", entry);
			}
		}
	}

}