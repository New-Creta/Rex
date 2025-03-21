#pragma once

#include "rex_engine/gfx/shader_reflection/shader_arithmetic_type.h"

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		// Reflection of a shader parameter, either input our out of a shader
		struct ShaderIODeclaration
		{
			rsl::tiny_stack_string semantic_name;		// The name of the shader parameter
			s32 semantic_index;							// The index of the semantic used for the parameter
			ShaderArithmeticType type;					// The type of the parameter
			s32 size;									// The size in bytes of the parameter
		};
	}
}