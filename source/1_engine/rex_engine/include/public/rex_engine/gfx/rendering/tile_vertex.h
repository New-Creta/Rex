#pragma once

#include "rex_std/bonus/math.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		struct TileVertex
		{
			rsl::point<f32> pos;
			rsl::point<f32> uv;
		};

		struct AnimatedTileVertex
		{
			glm::vec3 pos;
			rsl::point<f32> uv;
		};
	}
}