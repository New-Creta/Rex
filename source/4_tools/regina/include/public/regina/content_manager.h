#pragma once

#include "rex_std/string_view.h"

namespace regina
{
	class ContentManager
	{
	public:
		ContentManager();

		// Recursively add all the content of the given directory
		void add_content(rsl::string_view directory);

	private:
		rsl::vector<rsl::string> m_entries;
	};
}