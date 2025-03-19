#include "rex_engine/diagnostics/log.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/engine/project.h"

#include "rex_std/format.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: LOG PROJECT PATH

namespace rex
{
	TempString project_log_path()
	{
		return rex::path::join(rex::vfs::mount_path(rex::MountingPoint::Logs), rsl::format("{}.log", project_name()));
	}
}