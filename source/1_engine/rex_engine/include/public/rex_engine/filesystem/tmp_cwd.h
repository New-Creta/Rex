#pragma once

#include "rex_std/string_view.h"

namespace rex
{
	// Create a temporary working directory
	// The current working directory is changed to the specified directory on construction
	// the original working directory is set back on destruction
	class TempCwd
	{
	public:
		TempCwd(rsl::string_view directory);
		TempCwd(const TempCwd&) = delete;
		TempCwd(TempCwd&&) = delete;
		~TempCwd();

		TempCwd& operator=(const TempCwd&) = delete;
		TempCwd& operator=(TempCwd&&) = delete;

	private:
		rsl::string m_original_working_dir;
	};
}