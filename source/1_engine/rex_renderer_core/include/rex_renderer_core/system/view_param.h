#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/system/shader_elements.h"
#include "rex_renderer_core/system/shader_type.h"

namespace rex
{
	namespace gfx
	{
		class ResourceView;

		struct ViewOffset
		{
			s32 range_offset;
			s32 offset_within_range;
		};
	}
}