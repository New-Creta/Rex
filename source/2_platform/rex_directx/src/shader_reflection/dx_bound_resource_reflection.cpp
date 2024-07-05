#include "rex_directx/shader_reflection/dx_bound_resource_reflection.h"

namespace rex
{
	namespace gfx
	{
		DxBoundResourceReflection::DxBoundResourceReflection(const D3D12_SHADER_INPUT_BIND_DESC& resource_desc)
			: m_name(resource_desc.Name)
			, m_register(resource_desc.BindPoint)
		{}

		rsl::string_view DxBoundResourceReflection::name() const
		{
			return m_name;
		}
		card32 DxBoundResourceReflection::shader_register() const
		{
			return m_register;
		}
	}
}