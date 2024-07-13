#pragma once

#include "rex_renderer_core/shader_reflection/shader_param_type.h"

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		enum class ShaderParamComponentMask
		{
			X = 1,
			Y = 2,
			Z = 4,
			W = 8
		};

		struct ShaderParamReflDesc
		{
			rsl::tiny_stack_string semantic_name;
			s32 semantic_index;
			ShaderParameterType type;
			s32 size;
		};

		using ShaderParamReflection = ShaderParamReflDesc;
	}
}