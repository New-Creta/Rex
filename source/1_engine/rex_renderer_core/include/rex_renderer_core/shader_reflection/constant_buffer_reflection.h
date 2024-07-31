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
			rsl::tiny_stack_string name;								// The name of the constant buffer in the shader
			s32 size;																		// The size in bytes of the constant buffer
			s32 shader_register;												// The register the constant buffer is tied to
			s32 regiser_space;												  // The register space is used to define if its used by the renderpass or by a material
			rsl::vector<CBufferVarReflDesc> variables;	// The list of variables within the constant buffer
		};
	}
}