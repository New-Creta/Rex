#include "regina/content_manager.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"

#include "rex_engine/memory/memory_tracking.h"

#include "rex_engine/memory/memory_stats.h"

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

		m_entries = rex::directory::list_entries(directory, rex::directory::Recursive::yes);
	}

}