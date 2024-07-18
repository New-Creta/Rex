#include "rex_renderer_core/materials/parameters/material_parameter.h"

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
	}
}