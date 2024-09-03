#pragma once

#include "rex_std/string_view.h"
#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/shader_reflection/shader_class_type.h"
#include "rex_engine/gfx/shader_reflection/shader_variable_type.h"

namespace rex
{
	namespace gfx
	{
		// Reflection of a constant buffer variable, specified in a shader
		struct CBufferVarReflDesc
		{
			rsl::tiny_stack_string name;		// The name of the variable in the constant buffer
			s32 offset;											// The offset in bytes from the start of the constant buffer where the var is located
			s32 size;												// The size of the var, in bytes
			ShaderClassType class_type;			// The class type of the var
			ShaderVariableType var_type;		// The var type of the var
		};
	}
}