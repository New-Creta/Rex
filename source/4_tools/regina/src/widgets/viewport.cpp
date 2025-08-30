#include "regina/widgets/viewport.h"

#include "rex_engine/gfx/rendering/renderer.h"

#include "rex_engine/assets/tileset_asset.h"
#include "rex_engine/gfx/resources/render_target.h"
#include "rex_engine/gfx/graphics.h"

#include "imgui/imgui.h"
#include "rex_engine/gfx/imgui/imgui_utils.h"
#include "rex_engine/gfx/imgui/imgui_scoped_widget.h"

namespace regina
{
	Viewport::Viewport(rsl::string_view name, rsl::pointi32 resolution, const rex::Tilemap* tilemap, rex::TilesetAsset* tileset)
		: m_name(name)
		, m_tilemap(tilemap)
		, m_tileset(tileset)
		, m_screen_tile_resolution({ resolution.x / 8, resolution.y / 8 })
		, m_tile_zoom(1.0f)
	{
		m_render_target = rex::gfx::gal::instance()->create_render_target(resolution.x, resolution.y, rex::gfx::TextureFormat::Unorm4);
		m_render_target->debug_set_name("viewport render target");
		m_render_target_srv = rex::gfx::gal::instance()->create_srv(m_render_target.get());

		m_tile_render_pass = rsl::make_unique<rex::gfx::TileRenderPass>(m_render_target.get(), m_screen_tilemap.get(), m_tileset);
	}

	bool Viewport::on_update()
	{
		// based on the camera position, create a tilemap, the same size as the viewport's resolution
		// the camera has a zoom. more tiles are drawn depending on zoom
		m_tile_render_pass->set_tile_zoom(m_tile_zoom);
		m_screen_tile_resolution.x = (m_render_target->width() / 8.0f) / m_tile_zoom;
		m_screen_tile_resolution.y = (m_render_target->height() / 8.0f) / m_tile_zoom;

		rsl::pointi32 top_left = top_left_from_camera_pos(m_camera_pos);
		update_screen_tilemap(top_left);
		return false;
	}

	void Viewport::on_draw()
	{
		auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();
		m_tile_render_pass->render(render_ctx.get());

		if (auto widget = rex::imgui::ScopedWidget("Viewport"))
		{
			ImGui::GetCurrentWindow()->WindowClass.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_HiddenTabBar;
			ImGui::Text("This is the viewport");

			ImGui::DragInt("x: ", &m_camera_pos.x);
			ImGui::DragInt("y: ", &m_camera_pos.y);

			ImGui::DragFloat("zoom", &m_tile_zoom, 0.01f);

			ImVec2 imageSize{ (f32)m_render_target->width(), (f32)m_render_target->height() };
			ImGui::Image((ImTextureID)m_render_target_srv.get(), imageSize);
		}
	}

	void Viewport::set_tilemap(const rex::Tilemap* tilemap)
	{
		m_tilemap = tilemap;
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
		top_left.x -= m_screen_tile_resolution.x / 2;
		top_left.y -= m_screen_tile_resolution.y / 2;

		top_left.x = rsl::max(top_left.x, 0);
		top_left.y = rsl::max(top_left.y, 0);

		top_left.x = rsl::min(top_left.x, m_tilemap->width_in_tiles() - m_screen_tile_resolution.x);
		top_left.y = rsl::min(top_left.y, m_tilemap->height_in_tiles() - m_screen_tile_resolution.y);

		return top_left;
	}

	void Viewport::update_screen_tilemap(rsl::pointi32 topLeftStart)
	{
		if (m_screen_tilemap == nullptr || m_screen_tilemap->width_in_tiles() != m_screen_tile_resolution.x || m_screen_tilemap->height_in_tiles() != m_screen_tile_resolution.y)
		{
			m_screen_tilemap = rsl::make_unique<rex::Tilemap>(m_screen_tile_resolution.x, m_screen_tile_resolution.y);
			m_tile_render_pass->set_tilemap(m_screen_tilemap.get());
		}

		s32 num_tiles_until_end_of_row = m_tilemap->width_in_tiles() - topLeftStart.x;
		s32 num_to_copy = rsl::min(m_screen_tile_resolution.x, num_tiles_until_end_of_row);

		s32 start_idx = topLeftStart.y * m_tilemap->width_in_tiles() + topLeftStart.x;
		const u8* src = m_tilemap->tiles() + start_idx;
		s32 offset = 0;
		for (s32 row = 0; row < m_screen_tile_resolution.y; ++row)
		{
			m_screen_tilemap->set(src, num_to_copy, offset);
			offset += m_screen_tile_resolution.x;
			src += m_tilemap->width_in_tiles();
		}
	}
}