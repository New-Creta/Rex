#pragma once

#include "rex_renderer_core/materials/parameters/material_parameter.h"

namespace rex
{
	namespace gfx
	{
		class Resource;

		// A texture material parameter is a material parameter specifically used for samplers
		class SamplerMaterialParameter : public MaterialParameter
		{
		public:
			SamplerMaterialParameter(rsl::string_view name, ShaderType shaderType, s32 shaderRegister, Sampler2D* sampler = nullptr);

			// Update the internally stored sampler to a new sampler
			void set(Sampler2D* sampler);
			// Return the stored sampler for this parameter
			Sampler2D* sampler();

		private:
			Sampler2D* m_sampler;
		};
	}
}