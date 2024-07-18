#pragma once

#include "rex_renderer_core/shader_reflection/shader_param_type.h"

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		// Reflection of a shader parameter, either input our out of a shader
		struct ShaderParamReflection
		{
			rsl::tiny_stack_string semantic_name;		// The name of the shader parameter
			s32 semantic_index;											// The index of the semantic used for the parameter
			ShaderParameterType type;								// The type of the parameter
			s32 size;																// The size in bytes of the parameter
		};
	}
}