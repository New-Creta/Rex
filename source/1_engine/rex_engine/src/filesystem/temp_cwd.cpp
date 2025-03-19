#include "rex_engine/filesystem/tmp_cwd.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	TempCwd::TempCwd(rsl::string_view directory)
	{
		rsl::string_view abs_directory = rex::path::abs_path(directory);
		REX_ASSERT_X(directory::exists(abs_directory), "the directory specified to a temp working directory should exist. Directory: {}", abs_directory);
		m_original_working_dir.assign(rex::path::set_cwd(directory));
	}
	TempCwd::~TempCwd()
	{
		rex::path::set_cwd(m_original_working_dir);
	}
}