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
		, m_tile_zoom(1.0f)
		, m_camera_move_speed(1.0f)
	{
		init_render_pass();
	}

	bool Viewport::on_update()
	{
		// based on the camera position, create a tilemap, the same size as the viewport's resolution
		// the camera has a zoom. more tiles are drawn depending on zoom
		m_tile_render_pass->set_tile_zoom(m_tile_zoom);
;
		rsl::pointi32 top_left = top_left_from_camera_pos(m_camera_pos);
		update_screen_tilemap(top_left);
		return false;
	}

	void Viewport::on_draw()
	{
		auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();
		m_tile_render_pass->render(render_ctx.get());

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

				m_camera_pos.x += x_delta;
				m_camera_pos.y += y_delta;

				rsl::pointi32 resolution = screen_tile_resolution();
				s32 half_res_width = resolution.x / 2;
				s32 half_res_height = resolution.y / 2;
				m_camera_pos.x = rsl::clamp(m_camera_pos.x, half_res_width, m_world_tilemap->width_in_tiles() - half_res_width);
				m_camera_pos.y = rsl::clamp(m_camera_pos.y, half_res_height, m_world_tilemap->height_in_tiles() - half_res_height);
			}

			if (ImGui::IsWindowHovered())
			{
				if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftShift))
				{
					m_camera_move_speed += cam_speed_step * ImGui::GetIO().MouseWheel;
				}
				else
				{
					m_tile_zoom += zoom_step * ImGui::GetIO().MouseWheel;
				}
			}

			m_last_frame_mouse_pos = current_mouse_pos;
			m_tile_zoom = rsl::clamp(m_tile_zoom, zoom_min, zoom_max);
			m_camera_move_speed = rsl::clamp(m_camera_move_speed, cam_speed_min, cam_speed_max);

			ImGui::GetCurrentWindow()->WindowClass.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_HiddenTabBar;
			ImGui::Text("camera pos: (x: %d, y: %d)", m_camera_pos.x, m_camera_pos.y);
			ImGui::DragFloat("zoom: ", &m_tile_zoom, zoom_step, zoom_min, zoom_max, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat("camera speed: ", &m_camera_move_speed, cam_speed_step, cam_speed_min, cam_speed_max, "%.1f", ImGuiSliderFlags_AlwaysClamp);

			ImVec2 imageSize{ (f32)m_render_target->width(), (f32)m_render_target->height() };
			ImGui::Image((ImTextureID)m_render_target_srv.get(), imageSize);
		}
	}

	void Viewport::set_tilemap(const rex::Tilemap* tilemap)
	{
		m_world_tilemap = tilemap;
		init_render_pass();
	}
	void Viewport::set_tileset(const rex::TilesetAsset* tileset)
	{
		m_tileset = tileset;
		m_tile_render_pass->set_tileset(tileset);
	}
	void Viewport::set_camera_pos(rsl::pointi32 pos)
	{
		m_camera_pos = pos;
	}

	void Viewport::init_render_pass()
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
			rsl::clamp_max(m_world_tilemap->width_in_tiles(), max_tiles_in_width) * tile_size.x,
			rsl::clamp_max(m_world_tilemap->height_in_tiles(), max_tiles_in_height) * tile_size.y
		};

		if (!m_render_target || m_render_target->width() < resolution.x || m_render_target->height() < resolution.y)
		{
			m_render_target = rex::gfx::gal::instance()->create_render_target(resolution.x, resolution.y, rex::gfx::TextureFormat::Unorm4Srgb);
			m_render_target->debug_set_name("viewport render target");
			m_render_target_srv = rex::gfx::gal::instance()->create_srv(m_render_target.get());
			m_tile_render_pass = rsl::make_unique<rex::gfx::TileRenderPass>(m_render_target.get(), m_screen_tilemap.get(), m_tileset);
		}
	}

	rsl::pointi32 Viewport::top_left_from_camera_pos(rsl::pointi32 cameraPos)
	{
		// The camera position is in tile coordinates, which will always point to the middle of the screen
		// The goal of this function is to return the tile coordinate in the top left of the screen
		// This gets calculated by converting the viewport width in pixels to viewport width in tiles 
		// using the current zoom level and the number of pixels a single tile takes on screen.
		// Using this information, we subtract half the width and height from the camera pos
		// so it points to the top left of the screen

		// put the camera pos to this to make the top left of the viewport the top left of pallet town
		rsl::pointi32 top_left = cameraPos;
		rsl::pointi32 screen_resolution = screen_tile_resolution();
		top_left.x -= screen_resolution.x / 2;
		top_left.y -= screen_resolution.y / 2;

		top_left.x = rsl::max(top_left.x, 0);
		top_left.y = rsl::max(top_left.y, 0);

		top_left.x = rsl::min(top_left.x, m_world_tilemap->width_in_tiles() - screen_resolution.x);
		top_left.y = rsl::min(top_left.y, m_world_tilemap->height_in_tiles() - screen_resolution.y);

		return top_left;
	}

	void Viewport::update_screen_tilemap(rsl::pointi32 topLeftStart)
	{
		rsl::pointi32 screen_resolution = screen_tile_resolution();

		if (m_screen_tilemap == nullptr || m_screen_tilemap->width_in_tiles() != screen_resolution.x || m_screen_tilemap->height_in_tiles() != screen_resolution.y)
		{
			m_screen_tilemap = rsl::make_unique<rex::Tilemap>(screen_resolution.x, screen_resolution.y);
			m_tile_render_pass->set_tilemap(m_screen_tilemap.get());
		}

		s32 num_tiles_until_end_of_row = m_world_tilemap->width_in_tiles() - topLeftStart.x;
		s32 num_to_copy = rsl::min(screen_resolution.x, num_tiles_until_end_of_row);

		s32 start_idx = topLeftStart.y * m_world_tilemap->width_in_tiles() + topLeftStart.x;
		const u8* src = m_world_tilemap->tiles() + start_idx;
		s32 offset = 0;
		for (s32 row = 0; row < screen_resolution.y; ++row)
		{
			m_screen_tilemap->set(src, num_to_copy, offset);
			offset += screen_resolution.x;
			src += m_world_tilemap->width_in_tiles();
		}
	}

	rsl::pointi32 Viewport::screen_tile_resolution() const
	{
		rsl::pointi32 tile_resolution{};

		tile_resolution.x = m_render_target->width() / 8 / m_tile_zoom;
		tile_resolution.y = m_render_target->height() / 8 / m_tile_zoom;

		tile_resolution.x = rsl::clamp_max(tile_resolution.x, m_world_tilemap->width_in_tiles());
		tile_resolution.y = rsl::clamp_max(tile_resolution.y, m_world_tilemap->height_in_tiles());

		return tile_resolution;
	}
}