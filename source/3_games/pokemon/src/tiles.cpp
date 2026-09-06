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
		// We load 1 vram row at a tile from the tilemapD
		rsl::div_result res = rsl::div<s32>(dstStartIdx, constants::g_vram_tilemap_width);
		s32 dst_start = res.quot * (constants::g_vram_tilemap_width * constants::g_tile_size_px) + res.rem * (constants::g_tile_size / constants::g_tile_height_px);
		s32 src_start = 0;
		
		s32 dst_pixel_row_size = constants::g_vram_tilemap_width;
		s32 src_pixel_row_size = tileset->row_width;

		s32 dst_batch_size = constants::g_tile_width_px * dst_pixel_row_size;
		s32 src_batch_size = constants::g_tile_width_px * src_pixel_row_size;

		constexpr s32 src_tile_row_width = 2;
		constexpr s32 dst_tile_row_width = 16;
		




		s32 tile_idx_being_copied = 0;

		constexpr s32 tile_px_row_size = constants::g_tile_size / constants::g_tile_width_px;
		s32 num_tiles = tileset->num_tiles();
		for (s32 tile_idx = 0; tile_idx < num_tiles; ++tile_idx)
		{
			for (s32 i = 0; i < constants::g_tile_height_px; ++i)
			{
				s32 dst_tile_start = dst_start + batch_start + i * constants::g_vram_tilemap_width * tile_px_row_size;
				s32 src_tile_start = src_start;
			}
		}


		// Now we have to ensure we only copy 1 vram tilemap row at a time
		s32 num_tile_batches = 1 + tileset->num_tiles / constants::g_vram_tilemap_width;
		for (s32 tile_batch_num = 0; tile_batch_num < num_tile_batches; ++tile_batch_num)
		{
			s32 dst_batch_start = tile_batch_num * dst_batch_size;
			s32 src_batch_start = tile_batch_num * src_batch_size;
			for (s32 i = 0; i < constants::g_tile_height_px; ++i)
			{
				s32 dst_offset = dst_start + batch_start + i * constants::g_vram_tilemap_width;
				s32 src_offset = src_start + i * src_pixel_row_size;

				void* dst = m_tilemap + dst_offset;

				s32 src_offset = 0;
				rsl::memcpy(dst, tileset.data + src_offset, constants::g_tile_width_px);
				dst += constants::g_tile_width_px;
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