#pragma once

#include "rex_std/bonus/string.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		struct BoundResourceReflDesc
		{
			rsl::tiny_stack_string name;
			s32 shader_register;
		};

		class BoundResourceReflection
		{
		public:
			BoundResourceReflection(BoundResourceReflDesc&& desc);

		private:
			rsl::tiny_stack_string m_name;
			s32 m_shader_register;
		};
	}
}