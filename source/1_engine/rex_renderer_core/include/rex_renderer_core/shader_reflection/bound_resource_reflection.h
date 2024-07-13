#pragma once

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

#include "rex_renderer_core/system/shader_type.h"

namespace rex
{
	namespace gfx
	{
		enum class BoundResourceType
		{
			ConstantBuffer,
			Texture,
			Sampler
		};

		struct BoundResourceReflection
		{
		public:
			rsl::tiny_stack_string name;
			s32 shader_register;
			BoundResourceType resource_type;
			ShaderType shader_type;
		};
	}
}