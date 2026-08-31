#include "pokemon/tile_renderer.h"

namespace pokemon
{
	void TileRenderer::set_indices(s16 x, s16 y, rsl::initializer_list<s32> indices)
	{

	}

	namespace tile_renderer
	{
		rex::globals::GlobalUniquePtr<TileRenderer> g_tile_renderer;

		void init(rex::globals::GlobalUniquePtr<TileRenderer> tileRenderer)
		{
			g_tile_renderer = rsl::move(tileRenderer);
		}
		TileRenderer* get()
		{
			return g_tile_renderer.get();
		}
		void shutdown()
		{
			g_tile_renderer.reset();
		}
	}
}