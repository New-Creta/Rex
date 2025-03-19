#pragma once

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/system/shader_type.h"
#include "rex_engine/gfx/system/shader_parameter.h"

namespace rex
{
	namespace gfx
	{
		// The information about the bound resource
		// A resource can be of a few types, but is usually a constant buffer, texture or sampler
		struct ShaderResourceDeclaration
		{
		public:
			rsl::tiny_stack_string name;      			// The variable name of the resource
			s32 shader_register;						// The register the bound resource is tied to
			s32 register_space;	  						// The register space is used to determine if this is bound to a material or render pass
			ShaderParameterType resource_type;			// The type of the resource that's bound
			ShaderType shader_type;						// The shader type the resource is bound to
		};
	}
}