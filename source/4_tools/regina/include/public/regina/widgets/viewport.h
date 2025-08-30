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
	class Viewport : public Widget
	{
	public:
		Viewport(rsl::string_view name, rsl::pointi32 resolution, const rex::Tilemap* tilemap, rex::TilesetAsset* tileset);

		void set_tilemap(const rex::Tilemap* tilemap);
		void set_tileset(const rex::TilesetAsset* tileset);
		void set_camera_pos(rsl::pointi32 pos);

	protected:
		bool on_update() override;
		void on_draw();

	private:
		rsl::pointi32 top_left_from_camera_pos(rsl::pointi32 cameraPos);
		void update_screen_tilemap(rsl::pointi32 topLeftStart);

	private:
		// Give a name to the viewport, making it easier to identify
		rsl::string m_name;

		// The tileset to use for the viewport
		// We currently only support 1 tileset per viewport
		const rex::TilesetAsset* m_tileset;

		// the tilemap of the entire world, only a subset of this is rendered at a time
		const rex::Tilemap* m_tilemap;

		// The tilemap off the render target itself, which is a subset of the tilemap of the entire world
		rsl::unique_ptr<rex::Tilemap> m_screen_tilemap;

		// The current camera position in the world, offsetted from the top left of the world
		rsl::pointi32 m_camera_pos;
		rsl::pointi32 m_screen_tile_resolution;

		rsl::unique_ptr<rex::gfx::RenderTarget> m_render_target;
		rsl::unique_ptr<rex::gfx::ResourceView> m_render_target_srv;

		rsl::unique_ptr<rex::gfx::TileRenderPass> m_tile_render_pass;
	};
}