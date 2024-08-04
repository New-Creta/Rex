#include "rex_renderer_core/materials/parameters/mat_texture_parameter.h"

namespace rex
{
	namespace gfx
	{
		TextureMaterialParameter::TextureMaterialParameter(rsl::string_view name, ShaderType shaderType, s32 shaderRegister, Texture2D* texture)
			: MaterialParameter(name, shaderType, shaderRegister, MaterialParameterType::Texture)
			, m_texture(rsl::move(texture))
		{

		}

		void TextureMaterialParameter::set(Texture2D* texture)
		{
			m_texture = texture;
		}
		Texture2D* TextureMaterialParameter::texture()
		{
			return m_texture;
		}

	}
}