#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/globals.h"

#include "rex_engine/gfx/resources/upload_buffer.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"

#include "rex_engine/gfx/rendering/render_pass.h"

#include "glm/glm.hpp"

namespace rex
{
	namespace gfx
	{
		class Texture2D;
		class UnordedAccessBuffer;
		class RenderContext;
		class VertexBuffer;
		class IndexBuffer;
		class ConstantBuffer;
	}
}

namespace pokemon
{
	struct TilemapRenderingMetaData
	{
		u32 screen_tiles_per_row;							// the number of tiles per tile row on the screen
		u32 tilemap_tiles_per_row;						// the number of tiles per tile row in the texture

		glm::vec2 inv_tile_screen_size;				// the inverse of the width of a single tile on the screen
		glm::vec2 inv_pixel_screen_size;			// the inverse size of a single vintage pixel

		glm::vec2 tile_tilemap_size;					// the width and height of a single tile in the tilemap
		glm::vec2 inv_tile_tilemap_size;			// the inverse width and height of a single tile in the tilemap

		rsl::point<s32> screen_pixel_offset;	// the extra offset of a tile using vintage pixel size
	};

	class BackgroundTilesPass : public rex::gfx::RenderPass
	{
	public:
		BackgroundTilesPass();

		// Loads a tileset into vram at a location.
		// that location being an index
		s16 load(rsl::string_view tilesetPath, s16 dstStartIdx);
		s16 load(rsl::string_view tilesetPath, s16 dstStartIdx, s16 srcStartTileIdx, s16 srcNumTileIdx);
		
		// Set tile indices to be rendered at coordinate
		void set_indices(s16 x, s16 y, rsl::initializer_list<s32> indices);

		void run_pass(rex::gfx::RenderContext* ctx) override;

	private:
		rex::gfx::RenderPassDesc create_desc() const;

		void init_gfx_resources();
		void init_vb(rex::gfx::RenderContext* ctx);
		void init_ib(rex::gfx::RenderContext* ctx);
		void init_shader_params();
		void init_tile_render_info();

		rex::gfx::UnorderedAccessBuffer* active_tilemap();

	private:
		// The vertex buffer for a single tile
		// As we use instanced rendering, we only store 4 vertices
		rsl::unique_ptr<rex::gfx::VertexBuffer> m_vertex_buffer;
		
		// The index buffer for a single tile
		// It's just 6 indices, allowing us to make a single quad
		rsl::unique_ptr<rex::gfx::IndexBuffer> m_index_buffer;

		// The render info the shader needs for rendering
		// It's to store how big a single tile is, both on the tilemap 
		// as well as in the texture
		rsl::unique_ptr<rex::gfx::ConstantBuffer> m_tile_render_info;

		// This holds the tilemap on the GPU
		// Tile pixels are stored in 2bpp which is too small of a format
		// for modern GPUs, so we have to store them as a byte buffer
		// and have the data packed.
		// This also means we have to sample it ourselves
		rsl::unique_ptr<rex::gfx::UnorderedAccessBuffer> m_tilemap;

		// The indices of the tiles to render on screen
		// Starting from the top left and going to the bottom right
		rsl::unique_ptr<rex::gfx::UnorderedAccessBuffer> m_tile_indices_a;
		rsl::unique_ptr<rex::gfx::UnorderedAccessBuffer> m_tile_indices_b;

		TilemapRenderingMetaData m_metadata;
	};

	namespace tiles
	{
		void init(rex::globals::GlobalUniquePtr<BackgroundTilesPass> tileManager);
		BackgroundTilesPass* instance();
		void shutdown();
	}
}