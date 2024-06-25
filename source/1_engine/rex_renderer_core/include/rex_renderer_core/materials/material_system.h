#pragma once

#include "rex_renderer_core/materials/material.h"
#include "rex_std/string_view.h"

namespace rex
{
	namespace gfx
	{
		rsl::unique_ptr<Material> load_material(rsl::string_view filepath);
	}
}