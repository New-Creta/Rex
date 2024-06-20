#pragma once

#include "rex_std/string_view.h"
#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

#include "rex_renderer_core/shader_reflection/shader_class_type.h"
#include "rex_renderer_core/shader_reflection/shader_variable_type.h"

namespace rex
{
	namespace gfx
	{
		struct CBufferVarReflDesc
		{
			rsl::tiny_stack_string name;
			s32 offset;
			s32 size;
			ShaderClassType class_type;
			ShaderVariableType var_type;
		};

		class ConstantBufferVariableReflection
		{
		public:
			ConstantBufferVariableReflection(CBufferVarReflDesc&& desc);

			rsl::string_view name() const;

		private:
			rsl::tiny_stack_string m_name;
			s32 m_offset;
			s32 m_size;
			ShaderClassType m_class_type;
			ShaderVariableType m_variable_type;
		};
	}
}