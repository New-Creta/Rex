#include "pokemon/tiles.h"

#include "pokemon/memory_constants.h"

#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"

namespace pokemon
{
	TilesManager::TilesManager()
	{
		m_tilemap = rsl::make_unique<rex::gfx::Texture2D>(constants::g_tilemap_size);
		m_tile_indices = rsl::make_unique<rex::gfx::UnorderedAccessBuffer>(constants::g_tile_indices_size);
	}

	s16 TilesManager::load(rsl::string_view tilesetPath, s16 dstStartIdx)
	{
		return 0;
	}

	namespace tiles
	{


		rex::globals::GlobalUniquePtr<TilesManager> g_tile_manager;
		void init(rex::globals::GlobalUniquePtr<TilesManager> tileManager)
		{
			g_tile_manager = rsl::move(tileManager);
		}
		TilesManager* instance()
		{
			return g_tile_manager.get();
		}
		void shutdown()
		{
			g_tile_manager.reset();
		}
	}
}