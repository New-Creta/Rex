#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/assets/tilemap.h"
#include "rex_engine/assets/tileset_asset.h"
#include "rex_engine/assets/blockset.h"
#include "rex_std/string.h"
#include "rex_std/bonus/math.h"

#include "rex_engine/gfx/rendering/renderer.h"
#include "rex_engine/gfx/rendering/render_pass.h"
#include "rex_engine/gfx/rendering/render_passes/tile_pass.h"
#include "rex_engine/filesystem/path.h"

#include "rex_engine/assets/tileset_asset.h"
#include "rex_engine/gfx/resources/render_target.h"
#include "rex_engine/gfx/graphics.h"
#include "rex_engine/gfx/system/shader_library.h"

#include "imgui/imgui.h"
#include "rex_engine/gfx/imgui/imgui_utils.h"
#include "rex_engine/gfx/imgui/imgui_scoped_widget.h"

#include "regina/widgets/widget.h"

namespace rex
{
	namespace gfx
	{
		class RenderTarget;
		class ResourceView;
	}
}

namespace regina
{
	struct ViewportProps
	{
		const rex::TilesetAsset* tileset;
		const rex::Tilemap* tilemap;
	};

	class Viewport : public Widget
	{
	public:
		Viewport(const rex::Tilemap* tilemap, rex::TilesetAsset* tileset);

		void set_tilemap(const rex::Tilemap* tilemap);
		void set_tileset(const rex::TilesetAsset* tileset);
		void set_camera_pos(rsl::pointi32 pos);

		

	protected:
		bool on_update() override;
		void on_draw();

	private:
		void init_render_pass();

		rsl::pointi32 top_left_from_camera_pos(rsl::pointi32 cameraPos);
		void update_screen_tilemap(rsl::pointi32 topLeftStart);

		// The screen resolution but in tiles instead of pixels
		rsl::pointi32 screen_tile_resolution() const;

	private:
		// The tileset to use for the viewport
		// We currently only support 1 tileset per viewport
		const rex::TilesetAsset* m_tileset;

		// the tilemap of the entire world, only a subset of this is rendered at a time
		const rex::Tilemap* m_world_tilemap;

		// The tilemap off the render target itself, which is a subset of the tilemap of the entire world
		rsl::unique_ptr<rex::Tilemap> m_screen_tilemap;

		// The current camera position in the world, offsetted from the top left of the world
		rsl::pointi32 m_camera_pos;

		// The render target we're rendering to
		rsl::unique_ptr<rex::gfx::RenderTarget> m_render_target;
		// The render target's view as shader resource view, so it can be used in a shader
		rsl::unique_ptr<rex::gfx::ResourceView> m_render_target_srv;

		// The actual render pass used for rendering
		rsl::unique_ptr<rex::gfx::TileRenderPass> m_tile_render_pass;

		// Config settings that affect how much we display and how the user can interact with the viewport
		// ------------------------------------

		// The zoom of a tile, higher values show less tiles, smaller values show more tiles
		f32 m_tile_zoom;

		// the mouse position of the last frame
		// this is needed to support mouse dragging and moving the camera
		ImVec2 m_last_frame_mouse_pos;

		// The movement speed of the camera, higher values move the camera faster
		f32 m_camera_move_speed;
	};
}