#pragma once

#include "rex_renderer_core/shader_reflection/constant_buffer_variable_reflection.h"

namespace rex
{
	namespace gfx
	{
		struct CBufferReflDesc
		{
			rsl::tiny_stack_string name;
			s32 size;
			s32 shader_register;
			rsl::vector<ConstantBufferVariableReflection> variables;
		};

		class ConstantBufferReflection
		{
		public:
			ConstantBufferReflection(CBufferReflDesc&& desc);

		private:
			rsl::tiny_stack_string m_name;
			s32 m_size;
			s32 m_shader_register;
			rsl::vector<ConstantBufferVariableReflection> m_variables;
		};
	}
}