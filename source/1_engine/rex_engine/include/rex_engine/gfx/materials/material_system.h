#pragma once

#include "rex_engine/gfx/materials/material.h"
#include "rex_std/string_view.h"

#include "rex_engine/gfx/resources/blend_state.h"
#include "rex_engine/gfx/resources/raster_state.h"
#include "rex_engine/gfx/resources/depth_stencil_state.h"

namespace rex
{
	namespace gfx
	{
		// Load a material from disk, given a filepath
		rsl::unique_ptr<Material> load_material(rsl::string_view filepath);
	}
}