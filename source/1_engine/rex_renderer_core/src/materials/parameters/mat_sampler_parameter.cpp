#include "rex_renderer_core/materials/parameters/mat_sampler_parameter.h"

namespace rex
{
	namespace gfx
	{
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