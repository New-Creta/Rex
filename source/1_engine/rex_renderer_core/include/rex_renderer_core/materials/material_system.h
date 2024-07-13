#pragma once

#include "rex_renderer_core/materials/material.h"
#include "rex_std/string_view.h"

namespace rex
{
	namespace gfx
	{
		struct MaterialConstructSettings
		{
			bool color_normalized; // Is the color vertex input expected to be normalized or not?
		};

		rsl::unique_ptr<Material> load_material(rsl::string_view filepath);
	}
}