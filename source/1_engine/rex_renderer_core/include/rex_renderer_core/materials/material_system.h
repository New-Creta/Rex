#pragma once

#include "rex_renderer_core/materials/material.h"
#include "rex_std/string_view.h"

#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"

namespace rex
{
	namespace gfx
	{
		// Material construct settings are a wrapper around all the settings a material needs to store
		struct MaterialConstructSettings
		{
			RasterStateDesc raster_state;   // the rasterizer settings that'll be stored inside the material
			BlendDesc blend;								// the blend settings that'll be stored inside the material
			DepthStencilDesc depth_stencil;	// the depth stencil settings that'll be stored inside the material
		};

		// Load a material from disk, given a filepath
		rsl::unique_ptr<Material> load_material(rsl::string_view filepath);
	}
}