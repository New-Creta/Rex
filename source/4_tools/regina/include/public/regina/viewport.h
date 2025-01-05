#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/string.h"

namespace regina
{
	class Viewport
	{
	public:
		Viewport(rsl::string_view name);

	private:
		rsl::string m_name;
	};
}