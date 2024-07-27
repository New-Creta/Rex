#pragma once

#include "rex_renderer_core/resources/input_layout.h"

namespace rex
{
	namespace gfx
	{
		// Input layouts don't need to be created anymore in recent graphics APIs
		// However we often need to keep the graphics API specific data around
		// That's why we'll cache these
		namespace input_layout_cache
		{
			InputLayout* load(const InputLayoutDesc& desc);
		}
	}
}