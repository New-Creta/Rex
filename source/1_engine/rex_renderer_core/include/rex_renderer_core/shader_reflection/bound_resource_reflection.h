#pragma once

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

#include "rex_renderer_core/system/shader_type.h"

namespace rex
{
	namespace gfx
	{
		class BoundResourceReflection
		{
		public:
			BoundResourceReflection(const rsl::tiny_stack_string& name, s32 shaderRegister);

			rsl::string_view name() const;
			s32 shader_register() const;
			ShaderType shader_type() const;

		private:
			rsl::tiny_stack_string m_name;
			s32 m_shader_register;
			ShaderType m_shader_type;
		};
	}
}