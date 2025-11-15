#pragma once

#include "rex_engine/assets/tilemap.h"
#include "rex_engine/assets/tileset_asset.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/math/coords.h"

#include "rex_engine/gfx/resources/render_target.h"
#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"
#include "rex_engine/gfx/rendering/render_pass.h"
#include "rex_engine/gfx/rendering/render_params.h"

#include "rex_engine/gfx/system/render_context.h"

#include "rex_std/bonus/math.h"

#include "rex_std/memory.h"

// A block render pass takes in a number of block indices
// converts these into a tilemap and then sents the tilemap to the gpu

namespace rex
{
	namespace gfx
	{
		struct BlockRenderPassCreationInfo
		{
			RenderTargetBase* render_target;
		};

		struct TilemapRenderingMetaData
		{
			// Tile texture data
			u32 texture_tiles_per_row;   // the number of tiles per row in the tileset texture
			f32 inv_texture_width;       // the inverse width of the tileset texture, in pixels
			f32 inv_texture_height;      // the inverse height of the tileset texture, in pixels

			// Render target data
			u32 screen_width_in_tiles;   // the number of tiles we render on a single row
			f32 inv_tile_screen_width;   // the inverse of the width of a single tile on the screen
			f32 inv_tile_screen_height;  // the inverse of the height of a single tile on the screen

			int screen_pixel_offset_x;
			int screen_pixel_offset_y;
			float inv_pixel_screen_width;
			float inv_pixel_screen_height;
		};

		class BlockRenderPass : public RenderPass
		{
		public:
			BlockRenderPass(const BlockRenderPassCreationInfo& creationInfo);

			void update_params(const SceneRenderParams& params);

			//void update_scene_params(const SceneParams& params);
			//void update_camera_params(const CameraParams& params);

			// Update the tilemap's indices that we need to draw to the screen
			//void update_tilemap(const SceneRenderParams& params);

			void render(rex::gfx::RenderContext* renderCtx);

		private:
			RenderPassDesc create_desc(const BlockRenderPassCreationInfo& creationInfo) const;

			void init();

			void init_vb(rex::gfx::RenderContext* renderCtx);
			//void init_render_info(rex::gfx::RenderContext* renderCtx);
			void init_ib(rex::gfx::RenderContext* renderCtx);
			void init_tile_indices_uab(rex::gfx::RenderContext* renderCtx);
			void init_tilemap(const Camera2D* camera, const TilesetAsset* tileset);
			void init_shader_params();

			rsl::point<TileCount> calc_screen_resolution(const Camera2D* camera, const TilesetAsset* tileset) const;

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
			rsl::unique_ptr<rex::Tilemap> m_screen_tilemap;

			rex::gfx::RenderTargetBase* m_render_target;

			//SceneParams m_scene_params;
			
			TilemapRenderingMetaData m_render_metadata;

		};
	}
}