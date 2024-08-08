#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/system/view_param.h"

namespace rex
{
	namespace gfx
	{
		struct ShaderParameterLocation
		{
			s32 slot;
			s32 idx;
			ViewOffset view_offset;
		};
	}
}