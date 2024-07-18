#pragma once

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

#include "rex_renderer_core/system/shader_type.h"

namespace rex
{
	namespace gfx
	{
		// The resource that's bound to the shader
		enum class BoundResourceType
		{
			ConstantBuffer,
			Texture,
			Sampler
		};

		// The information about the bound resource
		struct BoundResourceReflection
		{
		public:
			rsl::tiny_stack_string name;      // The variable name of the resource
			s32 shader_register;							// The register the bound resource is tied to
			BoundResourceType resource_type;	// The type of the resource that's bound
			ShaderType shader_type;						// The shader type the resource is bound to
		};
	}
}