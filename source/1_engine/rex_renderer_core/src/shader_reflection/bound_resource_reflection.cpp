#include "rex_renderer_core/shader_reflection/bound_resource_reflection.h"

namespace rex
{
	namespace gfx
	{
		BoundResourceReflection::BoundResourceReflection(const rsl::tiny_stack_string& name, s32 shaderRegister)
			: m_name(name)
			, m_shader_register(shaderRegister)
		{

		}

		rsl::string_view BoundResourceReflection::name() const
		{
			return m_name;
		}
		s32 BoundResourceReflection::shader_register() const
		{
			return m_shader_register;
		}
	}
}