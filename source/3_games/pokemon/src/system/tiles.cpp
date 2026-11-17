#include "pokemon/system/tiles.h"

#include "pokemon/constants/memory_constants.h"
#include "pokemon/constants/render_constants.h"
#include "pokemon/system/registers.h"

#include "rex_engine/engine/asset_db.h"
#include "rex_engine/serialization/image_loading.h"

#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"
#include "rex_engine/gfx/graphics.h"

#include "rex_engine/gfx/system/shader_library.h"
#include "rex_engine/gfx/core/vertex.h"

#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"

#include "rex_std/bonus/cinttypes.h"


namespace pokemon
{
	using TileVertex = rex::gfx::VertexPos2DTex;


	BackgroundTilesPass::BackgroundTilesPass()
		: rex::gfx::RenderPass(create_desc())
		, m_metadata()
	{
		init_gfx_resources();
	}

	s16 BackgroundTilesPass::load(rsl::string_view tilesetPath, s16 dstStartIdx)
	{
		s16 start_idx = 0;
		s16 num_to_load = -1;
		return load(tilesetPath, dstStartIdx, start_idx, num_to_load);
	}
	s16 BackgroundTilesPass::load(rsl::string_view tilesetPath, s16 dstStartIdx, s16 srcStartTileIdx, s16 srcNumTileIdx)
	{
		REX_ASSERT_X(dstStartIdx < constants::g_tilemap_size / constants::g_tile_size, "destination index must be less than {} as that's how many tiles can possible be loaded into RAM");

		// Load the tileset data from disk into memory
		rex::memory::Blob content = rex::vfs::instance()->read_file(tilesetPath);

		// The image loaded from disk will always be in 8 bit mode
		// However, tiles are 2 bits per pixel, so we have to convert this data
		// from 8bpp to 2bpp.
		// Afterwards we need to upload the 2bpp data to the cpu
		// in a single buffer, NOT a texture, as that'd use too much memory
		// due to alignment and textures don't support 2bpp anymore on modern hardware
		rex::ImageLoadResult image_load_res = rex::load_image(content);

		REX_ASSERT_X(image_load_res.num_channels == 1, "Expecting only a single channel for tilesets. '{}' has more than 1 channel", tilesetPath);

		const u8 pixels_per_byte = 4;
		const u32 tiles_2bpp_data_size = image_load_res.data.byte_size() / pixels_per_byte;

		REX_ASSERT_X(tiles_2bpp_data_size <= constants::g_tilemap_size, "Trying to load too many tiles in 1 go. The limit is {}, you try to load {}", constants::g_tilemap_size, tiles_2bpp_data_size);

		auto ctx = rex::gfx::gal::instance()->new_render_ctx();
		rex::gfx::UploadDataAllocation upload_allocation = ctx->allocate_upload_region(tiles_2bpp_data_size);

		// Convert and load the tile data into a cpu only owned array
		// So we can copy this into the gpu using a single upload operation
		// instead of many small upload operations
		s32 offset = 0;
		for (s32 i = 0; i < image_load_res.data.elem_size(); i += 4)
		{
			u8 color0 = image_load_res.data[i + 0];
			u8 color1 = image_load_res.data[i + 1];
			u8 color2 = image_load_res.data[i + 2];
			u8 color3 = image_load_res.data[i + 3];

			// Convert the color to 2 bits per pixel
			const u8 color_diff = 256 / 4;
			u8 color0_2bpp = color0 / color_diff;
			u8 color1_2bpp = color1 / color_diff;
			u8 color2_2bpp = color2 / color_diff;
			u8 color3_2bpp = color3 / color_diff;

			// Pack the 2bpp colors together into 1
			u8 byte_color = color0_2bpp | color1_2bpp << 2 | color2_2bpp << 4 | color3_2bpp << 6;
			upload_allocation[offset++] = static_cast<rsl::byte>(byte_color);
		}

		ctx->upload_allocated_region(m_tilemap.get(), upload_allocation);
		return tiles_2bpp_data_size / constants::g_tile_size;
	}
	
	void BackgroundTilesPass::set_indices(s16 x, s16 y, rsl::initializer_list<s32> indices)
	{
		s32 start_idx = y * constants::g_screen_width_in_tiles + x;
		auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();
		render_ctx->update_buffer(active_tilemap(), indices.begin(), indices.size(), start_idx);
	}

	void BackgroundTilesPass::run_pass(rex::gfx::RenderContext* ctx)
	{
		bind_to(ctx);

		ctx->set_vertex_buffer(m_vertex_buffer.get());
		ctx->set_index_buffer(m_index_buffer.get());

		const s32 index_count_per_instance = m_index_buffer->count();
		const s32 instance_count = constants::g_screen_width_in_tiles * constants::g_screen_height_in_tiles;
		ctx->draw_indexed_instanced(index_count_per_instance, instance_count, 0, 0, 0);
	}

	rex::gfx::RenderPassDesc BackgroundTilesPass::create_desc() const
	{
		rex::gfx::RenderPassDesc desc{};

		desc.name = "Background Tiles Pass";

		desc.pso_desc.output_merger.raster_state = rex::gfx::gal::instance()->common_raster_state(rex::gfx::CommonRasterState::DefaultDepth);

		desc.framebuffer_desc.emplace_back(rex::gfx::gal::instance()->backbuffer_rendertarget());

		desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::instance()->load(rex::path::join(rex::engine::instance()->project_root(), "shaders", "tiles.hlsl"), rex::gfx::ShaderType::Vertex);
		desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::instance()->load(rex::path::join(rex::engine::instance()->project_root(), "shaders", "tiles.hlsl"), rex::gfx::ShaderType::Pixel);

		desc.pso_desc.input_layout = TileVertex::layout();

		return desc;
	}
	void BackgroundTilesPass::init_gfx_resources()
	{
		m_tilemap = rex::gfx::gal::instance()->create_unordered_access_buffer(constants::g_tilemap_size);
		m_tile_indices_a = rex::gfx::gal::instance()->create_unordered_access_buffer(constants::g_tile_indices_size);
		m_tile_indices_b = rex::gfx::gal::instance()->create_unordered_access_buffer(constants::g_tile_indices_size);

		auto ctx = rex::gfx::gal::instance()->new_render_ctx();
		init_vb(ctx.get());
		init_ib(ctx.get());
		init_shader_params();
		init_tile_render_info();
	}
	void BackgroundTilesPass::init_vb(rex::gfx::RenderContext* ctx)
	{
		// Init the vertex buffer
		const s32 num_vertices_per_tile = 4;

		rsl::array<TileVertex, num_vertices_per_tile> tile_vertices{};

		tile_vertices[0] = TileVertex{ glm::vec2(0,  0),    glm::vec2(0.0f, 0.0f) };
		tile_vertices[1] = TileVertex{ glm::vec2(1,	 0),    glm::vec2(1.0f, 0.0f) };
		tile_vertices[2] = TileVertex{ glm::vec2(0,	-1),	glm::vec2(0.0f, 1.0f) };
		tile_vertices[3] = TileVertex{ glm::vec2(1,	-1),	glm::vec2(1.0f, 1.0f) };

		m_vertex_buffer = rex::gfx::gal::instance()->create_vertex_buffer(num_vertices_per_tile, sizeof(TileVertex));

		ctx->update_buffer(m_vertex_buffer.get(), tile_vertices.data(), tile_vertices.size() * sizeof(TileVertex));
		ctx->transition_buffer(m_vertex_buffer.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);
	}
	void BackgroundTilesPass::init_ib(rex::gfx::RenderContext* ctx)
	{
		const s32 num_indices_per_tile = 6;
		rsl::array<u16, 6> tile_ib{};

		tile_ib[0] = 0;
		tile_ib[1] = 1;
		tile_ib[2] = 2;

		tile_ib[3] = 1;
		tile_ib[4] = 3;
		tile_ib[5] = 2;

		m_index_buffer = rex::gfx::gal::instance()->create_index_buffer(num_indices_per_tile, rex::gfx::IndexBufferFormat::Uint16);

		// create the constant buffer
		// -----------------------------------------
		ctx->update_buffer(m_index_buffer.get(), tile_ib.data(), tile_ib.size() * sizeof(tile_ib[0]));
		ctx->transition_buffer(m_index_buffer.get(), rex::gfx::ResourceState::IndexBuffer);
	}
	void BackgroundTilesPass::init_shader_params()
	{
		rex::gfx::Sampler2D* default_sampler = rex::gfx::gal::instance()->common_sampler(rex::gfx::CommonSampler::Default2D);
		set("default_sampler", default_sampler);
	}
	void BackgroundTilesPass::init_tile_render_info()
	{
		// This metadata requires updating as we are rendering slightly different now
		// We're not using a texture anymore for rendering tiles, we're using a byte buffer instead
		// This has an impact on UV calculation and sampling of the right color
		//
		// Also the screen width / height will always be the same, as we're using the way the game boy
		// rendering techniques to render tiles to the screen (although we render all tiles in 1 go, instead
		// of splitting this up into 3 instruction cycles).
		// We render 1 tiles outside of the immediate viewport to support for camera offset
		// when the camera is moving to have a smooth transition of the background
		
		// This is the resolution of the VRAM tiles buffer
		// As tiles get loaded into VRAM and from there, they get copied
		// into the render target
		// This is the resolution of the render target, represented in tiles
		m_metadata.screen_tiles_per_row = constants::g_screen_width_in_tiles;
		m_metadata.inv_pixel_screen_size = { 2.0f / constants::g_screen_width, 2.0f / constants::g_screen_height };
		m_metadata.inv_tile_screen_size = { 2.0f / constants::g_screen_width_in_tiles, 2.0f / constants::g_screen_height_in_tiles };

		m_metadata.tilemap_tiles_per_row = constants::g_vram_tilemap_width;
		m_metadata.tile_tilemap_size = { constants::g_vram_tilemap_width, constants::g_vram_tilemap_height };
		m_metadata.inv_tile_tilemap_size = { 1.0f / constants::g_vram_tilemap_width, 1.0f / constants::g_vram_tilemap_height };

		m_metadata.screen_pixel_offset = { 0, 0 };

		m_tile_render_info = rex::gfx::gal::instance()->create_constant_buffer(sizeof(TilemapRenderingMetaData), "Tilemap Rendering Metadata");
		set("RenderingMetaData", m_tile_render_info.get());

		auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();
		render_ctx->update_buffer(m_tile_render_info.get(), &m_metadata, sizeof(m_metadata));
	}

	rex::gfx::UnorderedAccessBuffer* BackgroundTilesPass::active_tilemap()
	{
		if (registers::g_lcdc.bg_tile_map || registers::g_lcdc.window_tilemap)
		{
			return m_tile_indices_b.get();
		}

		return m_tile_indices_a.get();
	}

	namespace tiles
	{
		rex::globals::GlobalUniquePtr<BackgroundTilesPass> g_tile_manager;
		void init(rex::globals::GlobalUniquePtr<BackgroundTilesPass> tileManager)
		{
			g_tile_manager = rsl::move(tileManager);
		}
		BackgroundTilesPass* instance()
		{
			return g_tile_manager.get();
		}
		void shutdown()
		{
			g_tile_manager.reset();
		}
	}
}