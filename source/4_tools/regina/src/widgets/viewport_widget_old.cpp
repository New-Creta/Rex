#include "regina/widgets/viewport_widget_old.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "rex_engine/gfx/graphics.h"
#include "rex_engine/gfx/imgui/imgui_scoped_widget.h"
#include "rex_engine/gfx/imgui/imgui_utils.h"

#include "rex_engine/engine/asset_db.h"

#include "rex_engine/filesystem/path.h"
#include "rex_std/bonus/math.h"

#include "rex_engine/gfx/rendering/renderer.h"
#include "rex_engine/gfx/rendering/render_pass.h"
#include "rex_engine/gfx/resources/sampler_2d.h"
#include "rex_engine/gfx/system/resource_cache.h"
#include "rex_engine/gfx/system/shader_library.h"

#include "rex_engine/shapes/rect.h"

#include "rex_engine/assets/blockset.h"
#include "rex_engine/assets/map.h"
#include "rex_engine/memory/blob_view.h"

namespace regina
{
	namespace constants
	{
		constexpr u8 g_map_padding_blocks = 3;

		constexpr u8 g_num_tiles_per_square_row = 2;
		constexpr u8 g_num_tiles_per_square_column = 2;
		constexpr u8 g_num_tiles_per_square = g_num_tiles_per_square_row * g_num_tiles_per_square_column;

		constexpr u8 g_num_tiles_per_block_row = 4;
		constexpr u8 g_num_tiles_per_block_column = 4;
		constexpr u8 g_num_tiles_per_block = g_num_tiles_per_block_row * g_num_tiles_per_block_column;

		constexpr u8 g_tile_width_px = 8;
		constexpr u8 g_tile_height_px = 8;
		constexpr u8 g_tile_size_px = g_tile_width_px * g_tile_height_px;

		constexpr u8 g_square_width_px = g_num_tiles_per_square_row * g_tile_width_px;
		constexpr u8 g_square_height_px = g_num_tiles_per_square_column * g_tile_height_px;
		constexpr u16 g_square_size_px = g_square_width_px * g_square_height_px;

		constexpr u8 g_block_width_px = g_num_tiles_per_block_row * g_tile_width_px;
		constexpr u8 g_block_height_px = g_num_tiles_per_block_column * g_tile_height_px;
		constexpr u16 g_block_size_px = g_block_width_px * g_block_height_px;

		constexpr u8 g_screen_width = 160;
		constexpr u8 g_screen_height = 144;

		constexpr u8 g_screen_width_in_tiles = g_screen_width / g_tile_width_px;
		constexpr u8 g_screen_height_in_tiles = g_screen_height / g_tile_height_px;

		constexpr u8 g_screen_width_in_squares = g_screen_width / g_square_width_px;
		constexpr u8 g_screen_height_in_squares = g_screen_height / g_square_height_px;

		constexpr u8 g_screen_width_in_blocks = g_screen_width / g_block_width_px;
		constexpr u8 g_screen_height_in_blocks = g_screen_height / g_block_height_px;
	}

	// This struct is the mapping of the constant buffer with the same name in the shader
	struct TileRenderingMetaData
	{
		// Tile texture data
		u32 texture_tiles_per_row;   // the number of tiles per row in the tileset texture
		f32 inv_texture_width;       // the inverse width of the tileset texture, in pixels
		f32 inv_texture_height;      // the inverse height of the tileset texture, in pixels

		// Render target data
		u32 screen_tiles_per_row;    // the number of tiles we render on a single row
		f32 inv_tile_screen_width;   // the inverse of the width of a single tile on the screen
		f32 inv_tile_screen_height;  // the inverse of the height of a single tile on the screen
	};

	struct TileRenderPassDesc
	{
		// The renderpass desc, holding the information the renderer needs to create the render pass
		rex::gfx::RenderPassDesc renderpass_desc;

		// Information about the tileset itself. This renderpass needs this information to render the tiles
		//TilesetDesc tileset_desc;

		// The actual block indices of the entire map. This is used to create the map matrix and pan over it
		rex::memory::BlobView block_indices;
	};

	// Returns the rect in which the connection map should be drawn
	rex::Rect8 rect_for_connection(const rex::Map* map, const rex::MapConnection& conn)
	{
		// Maps are always center aligned, their midpoint shifts depending on the connection offset
		s8 mid_point_x = constants::g_map_padding_blocks; // + ((map->width_in_blocks() / 2) + conn.offset);
		s8 mid_point_y = constants::g_map_padding_blocks; // + ((map->height_in_blocks() / 2) + conn.offset);

		s8 start = 0;
		s8 end = 0;
		
		// The formula to calculate the start is the same for each direction, just takes different input
		// depending on the connection's direction
		if (conn.direction == rex::Direction::North || conn.direction == rex::Direction::South)
		{
			s8 conn_half_width_in_blocks = (conn.map->desc().map_header.width_in_blocks / 2);
			s8 offsetted_start = mid_point_x - conn_half_width_in_blocks + conn.offset;
			s32 max_end = /*map->width_in_blocks() +*/ constants::g_map_padding_blocks;

			start = static_cast<s8>(rsl::clamp_min(offsetted_start, 0));
			end = rex::narrow_cast<s8>(rsl::min(max_end, start + conn.map->desc().map_header.width_in_blocks));
		}
		else
		{
			s8 conn_half_height_in_blocks = (conn.map->desc().map_header.height_in_blocks / 2);
			s8 offsetted_start = mid_point_y - conn_half_height_in_blocks + conn.offset;
			s32 max_end = /*map->height_in_blocks() +*/ constants::g_map_padding_blocks;

			start = static_cast<s8>(rsl::clamp_min(offsetted_start, 0));
			end = rex::narrow_cast<s8>(rsl::min(max_end, start + conn.map->desc().map_header.height_in_blocks));
		}

		// Fill in the rex::Rect8 based on the direction of the connection
		rex::Rect8 res{};

		switch (conn.direction)
		{
		case rex::Direction::North:
			res.top_left.x = start;
			res.top_left.y = 0;
			res.bottom_right.x = end;
			res.bottom_right.y = res.top_left.y + constants::g_map_padding_blocks;
			break;
		case rex::Direction::East:
			res.top_left.x = /*map->width_in_blocks() +*/ constants::g_map_padding_blocks;
			res.top_left.y = start;
			res.bottom_right.x = res.top_left.x + constants::g_map_padding_blocks;
			res.bottom_right.y = end;
			break;
		case rex::Direction::South:
			res.top_left.x = start;
			res.top_left.y = /*map->height_in_blocks() +*/ constants::g_map_padding_blocks;
			res.bottom_right.x = end;
			res.bottom_right.y = res.top_left.y + constants::g_map_padding_blocks;
			break;
		case rex::Direction::West:
			res.top_left.x = 0;
			res.top_left.y = start;
			res.bottom_right.x = res.top_left.x + constants::g_map_padding_blocks;
			res.bottom_right.y = end;
			break;
		}

		return res;
	}

	// Given a point in a map matrix, determine the coordinate of this point in the connecting map
	// let's say you have a map connecting to another in the north, both having an equal width and no offset
	// then a given coordinate {0, 0}, would return {0, height - constants::g_map_padding_blocks}
	rsl::pointi8 project_point_to_conn(const rex::Map* map, const rex::MapConnection& conn, rsl::pointi8 coord)
	{
		s8 projected_point = 0;
		if (conn.direction == rex::Direction::North || conn.direction == rex::Direction::South)
		{
			s8 mid_point_x = conn.map->desc().map_header.width_in_blocks / 2 - conn.offset;
			s8 offset_from_mid = constants::g_map_padding_blocks;// +(map->width_in_blocks() / 2) - coord.x;
			projected_point = mid_point_x - offset_from_mid;
		}
		else
		{
			s8 mid_point_y = conn.map->desc().map_header.height_in_blocks / 2 - conn.offset;
			s8 offset_from_mid = constants::g_map_padding_blocks;// +(map->height_in_blocks() / 2) - coord.y;
			projected_point = mid_point_y - offset_from_mid;
		}

		rsl::pointi8 res{};

		switch (conn.direction)
		{
		case rex::Direction::North:
			res.x = projected_point;
			res.y = conn.map->desc().map_header.height_in_blocks - constants::g_map_padding_blocks + coord.y;
			break;
		case rex::Direction::East:
			res.x = coord.x - constants::g_map_padding_blocks;// +map->width_in_blocks();
			res.y = projected_point;
			break;
		case rex::Direction::South:
			res.x = projected_point;
			res.y = coord.y - constants::g_map_padding_blocks;// +map->height_in_blocks();
			break;
		case rex::Direction::West:
			res.x = conn.map->desc().map_header.width_in_blocks - constants::g_map_padding_blocks + coord.x;
			res.y = projected_point;
			break;
		}

		return res;
	}

	s32 coord_to_index(rsl::pointi8 coord, s32 width)
	{
		return coord.y * width + coord.x;
	}
	rsl::pointi8 tile_to_block_coord(rsl::pointi8 coord)
	{
		rsl::pointi8 block_coord{};
		block_coord.x = coord.x / constants::g_num_tiles_per_block_row;
		block_coord.y = coord.y / constants::g_num_tiles_per_block_column;

		return block_coord;
	}
	void TileMap::build(const rex::Map* map)
	{
		//m_total_width = map->width_in_blocks() + (2 * constants::g_map_padding_blocks);
		//m_total_height = map->height_in_blocks() + (2 * constants::g_map_padding_blocks);

		//m_block_indices = rsl::make_unique<u8[]>(m_total_width * m_total_height);
		//init_blocks(map->border_block_idx());

		fill_connections(map);
		fill_inner_map(map);
	}

	TileMap::TileMap(s32 width, s32 height)
		: m_width(width)
		, m_height(height)
	{
		s32 num_tiles = width * height;
		m_tile_indices = rsl::make_unique<tile_idx[]>(num_tiles);
	}
	void TileMap::set(s32 row, s32 column, tile_idx tileIdx)
	{
		s32 offset = row * m_width + column;
		REX_ASSERT_X(offset < m_tile_indices.count(), "out of bounds access into the tilemap");

		m_tile_indices[offset] = tileIdx;
	}
	void TileMap::set(s32 row, s32 column, tile_idx* indices, s32 count)
	{
		s32 offset = row * m_width + column;
		REX_ASSERT_X(offset < m_tile_indices.count(), "out of bounds access into the tilemap");

		rsl::memcpy(&m_tile_indices[offset], indices, sizeof(tile_idx) * count);
	}


	s32 TileMap::width_in_blocks() const
	{
		return 0;
		//return m_total_width;
	}
	s32 TileMap::height_in_blocks() const
	{
		return 0;
		//return m_total_height;
	}
	s32 TileMap::major_dimension_in_blocks() const
	{
		return width_in_blocks();
	}
	s32 TileMap::width_in_tiles() const
	{
		return width_in_blocks() * constants::g_num_tiles_per_block_row;
	}
	s32 TileMap::height_in_tiles() const
	{
		return height_in_blocks() * constants::g_num_tiles_per_block_column;
	}
	s32 TileMap::major_dimension_in_tiles() const
	{
		return width_in_tiles();
	}

	s32 TileMap::index_at_blockcoord(rsl::pointi8 blockCoord) const
	{
		return 0;
		//s32 idx = coord_to_index(blockCoord, m_total_width);
		//return m_block_indices[idx];
	}

	s32 TileMap::index_at_tilecoord(rsl::pointi8 tileCoord) const
	{
		rsl::pointi8 block_coord = tile_to_block_coord(tileCoord);
		return index_at_blockcoord(block_coord);
	}

	void TileMap::init_blocks(s8 borderBlockIdx)
	{
		//rsl::fill_n(m_block_indices.begin(), m_block_indices.count(), borderBlockIdx);
	}
	void TileMap::fill_connections(const rex::Map* map)
	{
		//for (const rex::MapConnection& conn : map->connections())
		//{
		//	// Calculate the rect in which the connection will be drawn
		//	// This rect covers the entire padding region
		//	rex::Rect8 rect = rect_for_connection(map, conn);

		//	// Calculate the top left position where the drawing will start, inside of this rect
		//	rsl::pointi8 top_left_conn = project_point_to_conn(map, conn, rect.top_left);

		//	// Load the map blocks of the connection so we can assign the right block index to the map matrix
		//	//const rsl::unique_array<u8>& conn_map_blocks = rex::asset_db::instance()->load<rex::Map>(conn.map.name)->blocks();

		//	// Go over the blocks of the connection and assign the block index to the map matrix
		//	for (s8 y = rect.top_left.y, conn_y = top_left_conn.y; y < rect.bottom_right.y; ++y, ++conn_y)
		//	{
		//		for (s8 x = rect.top_left.x, conn_x = top_left_conn.x; x < rect.bottom_right.x; ++x, ++conn_x)
		//		{
		//			// Look up the block index of the current block we're looking at in the connection map
		//			s16 conn_idx = conn_y * conn.map.width_in_blocks + conn_x;
		//			u8 block_idx = conn_map_blocks[conn_idx];

		//			// Assign the connection's block index to the map matrix
		//			//s16 index = (y * m_total_width) + x;
		//			//m_block_indices[index] = block_idx;
		//		}
		//	}
		//}
	}
	void TileMap::fill_inner_map(const rex::Map* map)
	{
		//s32 height = map->height_in_blocks();
		//s32 width = map->width_in_blocks();

		//const rsl::unique_array<u8>& map_blocks = map->blocks();
		//for (s8 y = 0; y < height; ++y)
		//{
		//	for (s8 x = 0; x < width; ++x)
		//	{
		//		s32 index = y * width + x;
		//		s16 map_matrix_index = ((y + constants::g_map_padding_blocks) * m_total_width) + (x + constants::g_map_padding_blocks);
		//		m_block_indices[map_matrix_index] = map_blocks[index++];
		//	}
		//}
	}

	ViewportWidget::ViewportWidget(const ViewportWidgetDesc& desc)
		// These settings should be loaded from settings
		// As the user will likely want to start from the where they left the camera last time
		: m_camera_pos(desc.camera_pos)
	{
		// Initialize the render target early as this needs to be given to the render pass
		m_render_target = rex::gfx::gal::instance()->create_render_target(desc.width, desc.height, rex::gfx::TextureFormat::Unorm4Srgb);

		// Initialize a new render pass for the renderer, which will render the tiles of a map
		// TODO: Implement
		//rex::scratch_string tile_pass = rex::path::join(rex::engine::instance()->engine_root(), "renderpasses", "tilepass.json");
		//TileRenderPassDesc desc = rex::gfx::resource_cache::instance()->load_render_pass(tile_pass);

		TileRenderPassDesc renderpass_desc;
		renderpass_desc.renderpass_desc.name = "Viewport Tile Render Pass";
		renderpass_desc.renderpass_desc.framebuffer_desc.emplace_back(m_render_target.get());

		// This data should be initialized through data, not hardcoded
		renderpass_desc.renderpass_desc.pso_desc.output_merger.raster_state = rex::gfx::gal::instance()->common_raster_state(rex::gfx::CommonRasterState::DefaultDepth);
		renderpass_desc.renderpass_desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::instance()->load(rex::path::join(rex::engine::instance()->project_root(), "shaders", "render_tile_vertex.hlsl"), rex::gfx::ShaderType::Vertex);
		renderpass_desc.renderpass_desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::instance()->load(rex::path::join(rex::engine::instance()->project_root(), "shaders", "render_tile_pixel.hlsl"), rex::gfx::ShaderType::Pixel);
		renderpass_desc.renderpass_desc.pso_desc.input_layout =
		{
			// Per vertex data
			rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::Position, rex::gfx::ShaderArithmeticType::Float2 },
			rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::TexCoord, rex::gfx::ShaderArithmeticType::Float2 },
		};

		//renderpass_desc.tileset_desc = desc.tileset_desc;

		//m_render_pass = rex::gfx::renderer::instance()->add_render_pass<TilePass>(desc);
	}

	bool ViewportWidget::on_update()
	{
		process_input();
		update_renderpass_data();
		draw_imgui();

		return false;
	}

	void ViewportWidget::process_input()
	{
		// Process the input and update the camera accordingly
	}
	void ViewportWidget::update_renderpass_data()
	{
		bool any_input = false;

		// If there isn't any input, we don't have to update any of the static tiles in the tilemap
		if (any_input)
		{
			update_static_tiles();
		}



		//m_render_pass->update_camera(m_camera);
		//m_render_pass->reserve_objects(m_current_map->num_objects());
		//for (const auto& obj : MapObjectsRange(m_current_map))
		//{
		//	m_render_pass->push_object();
		//}
	}
	void ViewportWidget::draw_imgui()
	{
		ImGuiWindowFlags window_flags{};
		window_flags |= ImGuiWindowFlags_NoTitleBar;

		if (auto widget = rex::imgui::ScopedWidget("Viewport", nullptr, window_flags))
		{
			// Make sure that we hide the tab bar, it doesn't make sense to have it for our viewport
			ImGui::GetCurrentWindow()->WindowClass.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_HiddenTabBar;

			// Use the renderpass's render target as image for the viewport
			//const rex::gfx::RenderTarget* render_target = m_render_pass->render_target();
			//rex::imgui::image(render_target);
		}
	}
	void ViewportWidget::update_static_tiles()
	{

	}

}

