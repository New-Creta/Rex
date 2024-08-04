#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		struct ParamBindingIndices
		{
			s32 material_cbs = -1;
			s32 material_textures = -1;
			s32 material_samplers = -1;

			s32 renderpass_cbs = -1;
			s32 renderpass_textures = -1;
			s32 renderpass_samplers = -1;
		};
	}
}