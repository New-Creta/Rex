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
		// Load a material from disk, given a filepath
		rsl::unique_ptr<Material> load_material(rsl::string_view filepath);
	}
}