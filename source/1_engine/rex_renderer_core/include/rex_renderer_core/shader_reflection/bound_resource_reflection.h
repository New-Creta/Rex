#pragma once

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

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

		private:
			rsl::tiny_stack_string m_name;
			s32 m_shader_register;
		};
	}
}