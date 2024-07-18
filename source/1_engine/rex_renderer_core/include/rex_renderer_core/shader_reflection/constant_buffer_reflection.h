#pragma once

#include "rex_renderer_core/shader_reflection/constant_buffer_variable_reflection.h"
#include "rex_std/bonus/string.h"
#include "rex_std/vector.h"

namespace rex
{
	namespace gfx
	{
		// Reflection of a constant buffer, specified in a shader
		struct CBufferReflDesc
		{
			rsl::tiny_stack_string name;
			s32 size;
			s32 shader_register;
			rsl::vector<CBufferVarReflDesc> variables;
		};
	}
}