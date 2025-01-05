#pragma once

#include "rex_std/memory.h"

#include "pokemon/poke_structs.h"
#include "pokemon/map_matrix.h"
#include "pokemon/blockset.h"

#include "rex_engine/gfx/resources/texture_2d.h"

namespace pokemon
{
	struct MapRenderData
	{
		rsl::unique_ptr<BlockSet> blockset;
		rsl::unique_ptr<rex::gfx::Texture2D> tileset;
	};

	class Map
	{
	public:
		Map(MapMatrix&& mapMatrix, MapRenderData&& mapRenderData);

		const MapMatrix& map_matrix() const;
		const MapRenderData& render_data() const;

	private:
		MapMatrix m_map_matrix;
		MapRenderData m_map_render_data;
	};
}