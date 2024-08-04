#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/system/shader_elements.h"
#include "rex_renderer_core/system/shader_type.h"

namespace rex
{
	namespace gfx
	{
		struct ViewParam
		{
			s32 idx;
			DescriptorRangeType type;
			ShaderType shader_type;
		};
	}
}