#include "pokemon/tiles.h"

#include "pokemon/memory_constants.h"
#include "pokemon/render_constants.h"
#include "pokemon/tileset.h"

#include "rex_engine/engine/asset_db.h"

#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"
#include "rex_engine/gfx/graphics.h"

#include "rex_std/bonus/cinttypes.h"

namespace pokemon
{
	TilesManager::TilesManager()
	{
		s32 width = constants::g_tile_width_px * constants::g_vram_tilemap_width;
		s32 height = constants::g_tile_height_px * constants::g_vram_tilemap_height;

		m_tilemap = rex::gfx::gal::instance()->create_texture2d(width, height, rex::gfx::TextureFormat::Unorm4Srgb);
		m_tile_indices = rex::gfx::gal::instance()->create_unordered_access_buffer(constants::g_tile_indices_size);
	}

	s16 TilesManager::load(rsl::string_view tilesetPath, s16 dstStartIdx)
	{
		REX_ASSERT_X(dstStartIdx < constants::g_tilemap_size / constants::g_tile_size, "destination index must be less than {} as that's how many tiles can possible be loaded into RAM");

		// Load the tileset from disk
		Tileset* tileset = rex::asset_db::instance()->load<Tileset>(tilesetPath);

		// There is no guarantee that the tilemap on disk has the same resolution as the vram tilemap
		// Therefore we have to ensure we have the proper mapping between them
		constexpr s32 tile_px_row_size = constants::g_tile_size / constants::g_tile_width_px;
		s32 num_tiles = tileset->num_tiles;
		for (s32 tile_idx = 0; tile_idx < num_tiles; ++tile_idx)
		{
			rsl::div_result dst_div_res = rsl::div<s32>(tile_idx, constants::g_vram_tilemap_width);
			rsl::div_result src_div_res = rsl::div<s32>(tile_idx, tileset->row_width);

			s32 dst_start = dst_div_res.quot * constants::g_tile_size * constants::g_vram_tilemap_width + dst_div_res.rem * tile_px_row_size;
			s32 src_start = src_div_res.quot * constants::g_tile_size * tileset->row_width + src_div_res.rem * tile_px_row_size;

			for (s32 i = 0; i < constants::g_tile_height_px; ++i)
			{
				s32 dst_offset = dst_start + i * constants::g_vram_tilemap_width * tile_px_row_size;
				s32 src_offset = src_start + i * tileset->row_width * tile_px_row_size;

				void* dst = tilemap->data() + dst_offset;
				void* src = tileset->data + src_offset;

				rsl::memcpy(dst, src, tile_px_row_size);
			}
		}
	}

	void TilesManager::set_indices(s16 x, s16 y, rsl::initializer_list<s32> indices)
	{
		s32 start_idx = y * constants::g_screen_height_in_tiles + x;
		auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();
		render_ctx->update_buffer(m_tile_indices.get(), indices.begin(), indices.size(), start_idx);
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