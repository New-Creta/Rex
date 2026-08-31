#pragma once

#include "rex_engine/engine/globals.h"
#include "rex_engine/engine/types.h"

#include "pokemon/render_constants.h"

namespace pokemon
{
	class TileRenderer
	{
	public:
		void set_indices(s16 x, s16 y, rsl::initializer_list<s32> indices);

	private:
		rsl::array<s8, constants::g_screen_width_in_tiles * constants::g_screen_height_in_tiles> m_tile_map;
	};

	namespace tile_renderer
	{
		void init(rex::globals::GlobalUniquePtr<TileRenderer> tileRenderer);
		TileRenderer* instance();
		void shutdown();
	}
}