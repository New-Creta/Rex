#include "pokemon/tiles.h"

#include "pokemon/memory_constants.h"
#include "pokemon/render_constants.h"

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
		s8* data;

		rsl::div_result res = rsl::div<s32>(dstStartIdx, constants::g_vram_tilemap_width);
		s32 start = res.quot * constants::g_tile_size_px + res.rem * constants::g_tile_width_px;
		for (s32 i = 0; i < constants::g_vram_tilemap_height; ++i)
		{
			s32 dst = start + i * constants::g_vram_tilemap_width;
			rsl::memcpy(dst, data, constants::g_tile_width_px);
			dst += constants::g_tile_width_px;
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