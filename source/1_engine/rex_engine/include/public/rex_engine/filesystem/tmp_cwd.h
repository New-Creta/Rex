#pragma once

#include "rex_std/string_view.h"

namespace rex
{
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