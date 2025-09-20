#pragma once

#include "rex_engine/assets/tilemap.h"
#include "rex_engine/assets/blockmap.h"
#include "rex_engine/assets/tileset_asset.h"

#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/resources/render_target.h"
#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"
#include "rex_engine/gfx/rendering/render_pass.h"

#include "rex_engine/gfx/system/render_context.h"

#include "rex_std/bonus/math.h"

#include "rex_std/memory.h"

// A block render pass takes in a number of block indices
// converts these into a tilemap and then sents the tilemap to the gpu

namespace rex
{
	namespace gfx
	{
		struct BlockRenderPassParams
		{
			// pointer to the tile source data. This data represents all the tiles in the current world
			const u8* tiles_source;

			// the position (in tiles) from the world tiles from where we should start drawing
			rsl::pointi32 top_left_start;

			// The tilemap's resolution. If this resolution is different from the current tilemap, we create a new one
			// this is useful to have in case we want to zoom in or out
			rsl::pointi32 screen_resolution;

			// the width of the world, in tiles
			s32 world_width_in_tiles;
		};

		class BlockRenderPass
		{
		public:
			BlockRenderPass(rex::gfx::RenderTarget* rt, const rex::TilesetAsset* tileset);

			void set_render_target(RenderTarget* render_target);
			void set_tileset(const rex::TilesetAsset* tileset);
			void set_tile_zoom(f32 zoom);

			// Update the tilemap's indices that we need to draw to the screen
			void update_tilemap(const BlockRenderPassParams& params);

			void render(rex::gfx::RenderContext* renderCtx);

		private:
			void init();
			void init_vb(rex::gfx::RenderContext* renderCtx);
			void init_ib(rex::gfx::RenderContext* renderCtx);
			void init_render_info(rex::gfx::RenderContext* renderCtx);
			void init_tile_indices_uab(rex::gfx::RenderContext* renderCtx);
			void init_render_pass();

		private:
			rsl::unique_ptr<rex::gfx::VertexBuffer> m_tiles_vb_gpu;
			rsl::unique_ptr<rex::gfx::IndexBuffer> m_tiles_ib_gpu;
			rsl::unique_ptr<rex::gfx::ConstantBuffer> m_tile_render_info;
			rsl::unique_ptr<rex::gfx::UnorderedAccessBuffer> m_tiles_indices_buffer;
			rsl::unique_ptr<rex::gfx::RenderPass> m_render_pass;
			rsl::unique_ptr<rex::Tilemap> m_tilemap;
			rsl::unique_ptr<rex::Blockmap> m_blockmap;

			rex::gfx::RenderPassDesc m_render_pass_desc{};

			rex::gfx::RenderTarget* m_render_target;
			const rex::TilesetAsset* m_tileset;

			f32 m_tile_zoom;
		};
	}
}