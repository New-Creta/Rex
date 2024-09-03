#pragma once

#include "rex_engine/gfx/resources/input_layout.h"

namespace rex
{
	namespace gfx
	{
		// Input layouts don't need to be created anymore in recent graphics APIs
		// However we often need to keep the graphics API specific data around
		// That's why we'll cache these
		namespace input_layout_cache
		{
			// Load an input layout from the cache, or store a new one
			InputLayout* load(const InputLayoutDesc& desc);
			// Clear the cache
			void clear();
		}
	}
}