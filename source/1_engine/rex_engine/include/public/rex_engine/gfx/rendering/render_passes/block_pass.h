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

			// The screen's tilemap resolution. If this resolution is different from the current tilemap, we create a new one
			// this is useful to have in case we want to zoom in or out
			rsl::pointi32 screen_resolution;

			// the width of the world, in tiles
			s32 world_width_in_tiles;
		};

		class BlockRenderPass
		{
		public:
			BlockRenderPass(rex::gfx::RenderTargetBase* rt, const rex::TilesetAsset* tileset);

			void set_render_target(RenderTargetBase* render_target);
			void set_tileset(const rex::TilesetAsset* tileset);
			void set_tile_zoom(f32 zoom);

			// Update the tilemap's indices that we need to draw to the screen
			void update_tilemap(const BlockRenderPassParams& params);

			void render(rex::gfx::RenderContext* renderCtx);

		private:
			void init();

			void init_tile_info(rex::gfx::RenderContext* renderCtx);

			void init_vb(rex::gfx::RenderContext* renderCtx, rsl::vec2 inTileSize, rsl::vec2 uvSize);
			void init_render_info(rex::gfx::RenderContext* renderCtx, rsl::vec2 inTileSize, rsl::vec2 uvSize);
			void init_ib(rex::gfx::RenderContext* renderCtx);
			void init_tile_indices_uab(rex::gfx::RenderContext* renderCtx);
			void init_render_pass();

		private:
			// The vertex buffer for a single tile
			// As we use instanced rendering, we only store 4 vertices
			rsl::unique_ptr<rex::gfx::VertexBuffer> m_tiles_vb_gpu;
			// The index buffer for a single tile
			// It's just 6 indices, allowing us to make a single quad
			rsl::unique_ptr<rex::gfx::IndexBuffer> m_tiles_ib_gpu;

			// The render info the shader needs for rendering
			// It's to store how big a single tile is, both on the tilemap 
			// as well as in the texture
			rsl::unique_ptr<rex::gfx::ConstantBuffer> m_tile_render_info;

			// The UAV holding the indices of all the tiles to render
			// starting from the top left, going to the bottom right
			rsl::unique_ptr<rex::gfx::UnorderedAccessBuffer> m_tiles_indices_buffer;

			// Legacy data, this should be merged into a base class of this class
			rsl::unique_ptr<rex::gfx::RenderPass> m_render_pass;

			// The tilemap to render on screen.
			// This is the source information that gets copied to the UAV buffer
			rsl::unique_ptr<rex::Tilemap> m_tilemap;

			rex::gfx::RenderPassDesc m_render_pass_desc{};

			rex::gfx::RenderTargetBase* m_render_target;
			const rex::TilesetAsset* m_tileset;

			f32 m_tile_zoom;
		};
	}
}