#pragma once

#include "rex_renderer_core/materials/material.h"

#include "rex_renderer_core/resources/texture_2d.h"

namespace rex
{
	namespace gfx
	{
		class ImGuiMaterial
		{
		public:

		private:
			rsl::unique_ptr<Texture2D> m_fonts_texture;
		};
	}
}