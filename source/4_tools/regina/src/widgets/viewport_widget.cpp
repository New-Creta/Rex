#include "regina/widgets/viewport_widget.h"

#include "rex_engine/gfx/rendering/renderer.h"

#include "rex_engine/assets/tileset_asset.h"
#include "rex_engine/gfx/resources/render_target.h"
#include "rex_engine/gfx/graphics.h"

#include "imgui/imgui.h"
#include "rex_engine/gfx/imgui/imgui_utils.h"
#include "rex_engine/gfx/imgui/imgui_scoped_widget.h"

namespace regina
{
	DEFINE_LOG_CATEGORY(LogViewport);

	Viewport::Viewport(const rex::Tilemap* tilemap, rex::TilesetAsset* tileset)
		: m_world_tilemap(tilemap)
		, m_tileset(tileset)
		//, m_tile_zoom(1.0f)
		, m_camera_move_speed(1.0f)
	{
		//init_render_graph();
	}

	bool Viewport::on_update()
	{
		// based on the camera position, create a tilemap, the same size as the viewport's resolution
		// the camera has a zoom. more tiles are drawn depending on zoom
		//m_block_render_pass->set_tile_zoom(m_tile_zoom);
;
		rsl::point<rex::TileCount> top_left = top_left_from_camera_pos(m_camera_pos);
		update_screen_tilemap(top_left);
		return false;
	}

	void Viewport::on_draw()
	{
		auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();
		m_block_render_pass->render(render_ctx.get());

		if (auto widget = rex::imgui::ScopedWidget("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			f32 zoom_min = 1.0f;
			f32 zoom_max = 10.0f;
			f32 zoom_step = 0.125;
			f32 cam_speed_min = 0.1f;
			f32 cam_speed_max = 10.0f;
			f32 cam_speed_step = 0.1f;

			ImVec2 current_mouse_pos = ImGui::GetMousePos();
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
			{
				f32 x_delta = (m_last_frame_mouse_pos.x - current_mouse_pos.x) * m_camera_move_speed;
				f32 y_delta = (m_last_frame_mouse_pos.y - current_mouse_pos.y) * m_camera_move_speed;

				m_camera_pos.x += static_cast<s32>(x_delta);
				m_camera_pos.y += static_cast<s32>(y_delta);

				//rsl::pointi32 resolution = screen_tile_resolution();
				s32 half_res_width = m_screen_resolution.x.get() / 2;
				s32 half_res_height = m_screen_resolution.y.get() / 2;
				//m_camera_pos.x = rsl::clamp(m_camera_pos.x, half_res_width, m_world_tilemap->width().get() - half_res_width);
				//m_camera_pos.y = rsl::clamp(m_camera_pos.y, half_res_height, m_world_tilemap->height().get() - half_res_height);
			}

			if (ImGui::IsWindowHovered())
			{
				if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftShift))
				{
					m_camera_move_speed += cam_speed_step * ImGui::GetIO().MouseWheel;
				}
				else
				{
					m_screen_resolution.x.get() += 1;
					m_screen_resolution.y.get() += 1;
					//m_tile_zoom += zoom_step * ImGui::GetIO().MouseWheel;
				}
			}

			m_last_frame_mouse_pos = current_mouse_pos;
			//m_tile_zoom = rsl::clamp(m_tile_zoom, zoom_min, zoom_max);
			m_camera_move_speed = rsl::clamp(m_camera_move_speed, cam_speed_min, cam_speed_max);

			ImGui::GetCurrentWindow()->WindowClass.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_HiddenTabBar;
			ImGui::Text("camera pos: (x: %d, y: %d)", m_camera_pos.x, m_camera_pos.y);
			//ImGui::DragFloat("zoom: ", &m_tile_zoom, zoom_step, zoom_min, zoom_max, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat("camera speed: ", &m_camera_move_speed, cam_speed_step, cam_speed_min, cam_speed_max, "%.1f", ImGuiSliderFlags_AlwaysClamp);

			ImVec2 imageSize{ (f32)m_render_target->width(), (f32)m_render_target->height() };
			ImGui::Image((ImTextureID)m_render_target_srv, imageSize);
		}
	}

	void Viewport::set_tilemap(const rex::Tilemap* tilemap)
	{
		m_world_tilemap = tilemap;
		//init_render_graph();
	}
	void Viewport::set_tileset(const rex::TilesetAsset* tileset)
	{
		m_tileset = tileset;
		init_render_graph();

		rex::gfx::BlockRenderPassDynamicInputs inputs{};
		inputs.tileset = tileset;

		m_block_render_pass->update_dynamic_inputs(inputs);
		//m_block_render_pass->set_tileset(tileset);
	}
	void Viewport::set_camera_pos(rsl::pointi32 pos)
	{
		m_camera_pos = pos;
	}

	void Viewport::init_render_graph()
	{
		if (!m_world_tilemap)
		{
			return;
		}

		rsl::pointi8 tile_size = { 8, 8 };
		if (m_tileset)
		{
			tile_size = m_tileset->tile_size();
		}

		s32 max_tiles_in_width = 100;
		s32 max_tiles_in_height = 50;

		rsl::pointi32 resolution =
		{
			rsl::clamp_max(m_world_tilemap->width().get(), max_tiles_in_width)* tile_size.x,
			rsl::clamp_max(m_world_tilemap->height().get(), max_tiles_in_height)* tile_size.y
		};

		if (!m_render_target || m_render_target->width() < resolution.x || m_render_target->height() < resolution.y)
		{
			m_render_target = rex::gfx::gal::instance()->create_render_target(resolution.x, resolution.y, rex::gfx::TextureFormat::Unorm4Srgb);
			m_screen_resolution.x.get() = m_render_target->width() / 8;
			m_screen_resolution.y.get() = m_render_target->height() / 8;
			//m_render_target->debug_set_name("viewport render target");
			m_render_target_srv = rex::gfx::gal::instance()->create_srv(m_render_target.get());

			rex::gfx::BlockRenderPassDynamicInputs inputs{};
			inputs.render_target = m_render_target.get();
			inputs.tileset = m_tileset;
			inputs.screen_resolution = m_screen_resolution;
			m_block_render_pass = rsl::make_unique<rex::gfx::BlockRenderPass>(inputs);
		}
	}

	rsl::point<rex::TileCount> Viewport::top_left_from_camera_pos(rsl::pointi32 cameraPos)
	{
		// The camera position is in tile coordinates, which will always point to the middle of the screen
		// The goal of this function is to return the tile coordinate in the top left of the screen
		// This gets calculated by converting the viewport width in pixels to viewport width in tiles 
		// using the current zoom level and the number of pixels a single tile takes on screen.
		// Using this information, we subtract half the width and height from the camera pos
		// so it points to the top left of the screen

		// put the camera pos to this to make the top left of the viewport the top left of pallet town
		rsl::pointi32 top_left = cameraPos;
		//rsl::pointi32 screen_resolution = screen_tile_resolution();
		//top_left.x -= m_screen_resolution.x.get() / 2;
		//top_left.y -= m_screen_resolution.y.get() / 2;

		//top_left.x = rsl::max(top_left.x, 0);
		//top_left.y = rsl::max(top_left.y, 0);

		//top_left.x = rsl::min(top_left.x, m_world_tilemap->width().get() - m_screen_resolution.x.get());
		//top_left.y = rsl::min(top_left.y, m_world_tilemap->height().get() - m_screen_resolution.y.get());

		return rsl::point<rex::TileCount>{ rex::TileCount(top_left.x), rex::TileCount(top_left.y) };
	}

	void Viewport::update_screen_tilemap(rsl::point<rex::TileCount> topLeftStart)
	{
		rex::gfx::BlockRenderPassUpdateParams params{};
		//params.screen_resolution = screen_tile_resolution();
		params.tiles_source = m_world_tilemap->tiles();
		params.top_left_start = topLeftStart;
		params.world_width_in_tiles = m_world_tilemap->width().get();
		m_block_render_pass->update_tilemap(params);
	}
}