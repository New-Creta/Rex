#pragma once

#include "rex_engine/gfx/rendering/render_pass.h"

namespace rex
{
	namespace gfx
	{
		struct TilemapPassDynamicInputs
		{
			RenderTargetBase* render_target;
			const TilesetAsset* tileset;
			rsl::point<TileCount> screen_resolution;
		};

		struct TilemapPassUpdateParams
		{
			// pointer to the tile source data. This data represents all the tiles in the current world
			const u8* tiles_source;

			// the position (in tiles) from the world tiles from where we should start drawing
			rsl::point<TileCount> top_left_start;

			// the width of the world, in tiles
			s32 world_width_in_tiles;
		};

		class TilemapPass : public RenderPass
		{
		public:
			TilemapPass(const TilemapPassDynamicInputs& inputs);

			void update_dynamic_inputs(const TilemapPassDynamicInputs& inputs);

			// Update the tilemap's indices that we need to draw to the screen
			void update_tilemap(const TilemapPassUpdateParams& params);

			void render(rex::gfx::RenderContext* renderCtx);

		private:
			RenderPassDesc create_desc() const;

			void init();

			void init_vb(rex::gfx::RenderContext* renderCtx);
			void init_render_info(rex::gfx::RenderContext* renderCtx);
			void init_ib(rex::gfx::RenderContext* renderCtx);
			void init_tile_indices_uab(rex::gfx::RenderContext* renderCtx);
			void init_render_pass();
			void init_tilemap();

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

			// The tilemap to render on screen.
			// This is the source information that gets copied to the UAV buffer
			rsl::unique_ptr<rex::Tilemap> m_tilemap;

			rex::gfx::RenderTargetBase* m_render_target;
			const rex::TilesetAsset* m_tileset;
			rsl::point<TileCount> m_screen_resolution;
		};
	}
}