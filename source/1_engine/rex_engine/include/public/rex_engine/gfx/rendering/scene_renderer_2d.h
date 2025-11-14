#pragma once

#include "rex_std/memory.h"
#include "rex_std/vector.h"

#include "rex_engine/engine/globals.h"
#include "rex_engine/math/coords.h"

#include "rex_engine/gfx/rendering/renderer.h"

#include "rex_engine/gfx/rendering/render_passes/animated_sprites_pass.h"
#include "rex_engine/gfx/rendering/render_passes/block_pass.h"
#include "rex_engine/gfx/rendering/render_passes/composite_pass.h"

#include "rex_engine/gfx/resources/animated_sprite.h"
#include "rex_engine/gfx/resources/depth_stencil_buffer.h"

namespace rex
{
	namespace gfx
	{
		struct SceneRenderParams
		{
			const u8* tiles_source;
			PixelCoord top_left;
			WorldCoordConverter coord_converter;
			TileCount world_width_in_tiles;
			PixelCoord cam_top_left;
		};

		class SceneRenderer2D : public Renderer
		{
		public:
			SceneRenderer2D();

			void render() override;

			void add_animated_sprite(rsl::unique_ptr<AnimatedSprite> animatedSprite);

			void update_params(const SceneRenderParams& params);
			void update_zoom(rsl::point<f32> zoomLevel);
			void notify_new_tileset(const TilesetAsset* tileset);

		private:
			void render_tilemap(RenderContext* renderCtx);
			void render_flipbook_animations(RenderContext* renderCtx);
			void render_dynamic_animations(RenderContext* renderCtx);

		private:
			rsl::vector<rsl::unique_ptr<AnimatedSprite>> m_animated_sprites;

			rsl::point<TileCount> m_top_left;
			rex::Tilemap* m_scene_tilemap;

			rsl::unique_ptr<RenderTarget> m_render_target;

			rsl::unique_ptr<BlockRenderPass> m_block_render_pass;
			rsl::unique_ptr<AnimatedSpritesPass> m_animated_sprites_pass;
			rsl::unique_ptr<CompositePass> m_composite_pass;

			rsl::point<f32> m_zoom_level;
			SceneRenderParams m_params;
		};

		namespace scene_renderer
		{
			void init(globals::GlobalUniquePtr<SceneRenderer2D> renderer);
			SceneRenderer2D* instance();
			void shutdown();
		}
	}
}

