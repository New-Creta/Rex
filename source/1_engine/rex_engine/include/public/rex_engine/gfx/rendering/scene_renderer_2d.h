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

#include "rex_engine/gfx/rendering/camera_2d.h"

namespace rex
{
	namespace gfx
	{
		// structure holding all the required data to render a single frame
		// different render passes may use some of the data in here for their internal rendering logic
		struct SceneRenderParams
		{
			const TilesetAsset* tileset;					// tileset to be used for the active scene/world
			const u8* tiles_source;								// pointer to start of the buffer listing all tiles of the current scene, starting from the top left
			const Camera2D* camera;								// camera used by the current scene
			WorldCoordConverter coord_converter;  // converter used to calcualte coordinates of a different scale for the current scene
			TileCount world_width_in_tiles;				// the total width of the world, in tiles
		};

		//struct SceneRenderParams
		//{
		//	const u8* tiles_source;
		//	Camera2D* camera;
		//	WorldCoordConverter coord_converter;
		//	TileCount world_width_in_tiles;
		//};

		class SceneRenderer2D : public Renderer
		{
		public:
			SceneRenderer2D();

			// call into each render pass to render a single frame
			void render() override;

			// Add a new animated sprite to the renderer's representation of the world
			void add_animated_sprite(rsl::unique_ptr<AnimatedSprite> animatedSprite);

			// Update the scene render parameters
			void update_params(const SceneRenderParams& params);
			// Return the current parameters the renderer uses for the world
			SceneRenderParams current_params() const;

			//void update_zoom(rsl::point<f32> zoomLevel);
			//void notify_new_tileset(const TilesetAsset* tileset);

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

