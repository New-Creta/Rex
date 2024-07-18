#pragma once

#include "rex_renderer_core/materials/parameters/material_parameter.h"

namespace rex
{
	namespace gfx
	{
		// A texture material parameter is a material parameter specifically used for textures
		class TextureMaterialParameter : public MaterialParameter
		{
		public:
			TextureMaterialParameter(rsl::string_view name, ShaderType shaderType, s32 shaderRegister, Texture2D* texture = nullptr);

			// Update the internally stored texture to a new texture
			void set(Texture2D* texture);
			// Return the stored texture for this parameter
			Texture2D* texture();
			// Return the wrapped resource
			Resource* resource() override;

		private:
			Texture2D* m_texture;
		};
	}
}