#include "rex_renderer_core/materials/material_parameter.h"

#include "rex_engine/string/stringid.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/text_processing/text_processing.h"

namespace rex
{
	namespace gfx
	{
		MaterialParameter::MaterialParameter(rsl::string_view name, ShaderType shaderType, s32 shaderRegister, MaterialParameterType type)
			: m_name(name)
			, m_shader_register(shaderRegister)
			, m_shader_type(shaderType)
			, m_type(type)
		{
		}

		rsl::string_view MaterialParameter::name() const
		{
			return m_name;
		}
		s32 MaterialParameter::shader_register() const
		{
			return m_shader_register;
		}
		MaterialParameterType MaterialParameter::type() const
		{
			return m_type;
		}
		ShaderType MaterialParameter::shader_type() const
		{
			return m_shader_type;
		}


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

		SamplerMaterialParameter::SamplerMaterialParameter(rsl::string_view name, ShaderType shaderType, s32 shaderRegister, Sampler2D* sampler)
			: MaterialParameter(name, shaderType, shaderRegister, MaterialParameterType::Sampler)
			, m_sampler(rsl::move(sampler))
		{

		}

		void SamplerMaterialParameter::set(Sampler2D* sampler)
		{
			m_sampler = sampler;
		}
		Sampler2D* SamplerMaterialParameter::sampler()
		{
			return m_sampler;
		}
	}
}