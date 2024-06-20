#pragma once

#include "rex_renderer_core/shader_reflection/shader_param_type.h"

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		struct ShaderParamReflDesc
		{
			rsl::tiny_stack_string semantic_name;
			s32 semantic_index;
			ShaderParameterType param_type;
			s32 size;
		};

		class ShaderParamReflection
		{
		public:
			ShaderParamReflection(ShaderParamReflDesc&& desc);

		private:
			rsl::tiny_stack_string m_semantic_name;
			s32 m_semantic_index;
			ShaderParameterType m_type;
			s32 m_size;
		};
	}
}