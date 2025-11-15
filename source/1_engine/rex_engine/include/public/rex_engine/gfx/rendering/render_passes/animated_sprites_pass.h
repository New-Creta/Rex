#pragma once

#include "rex_engine/gfx/rendering/render_pass.h"

#include "rex_engine/assets/texture_asset.h"

namespace rex
{
	namespace gfx
	{
		struct AnimatedSpritePassCreationInfo
		{
			RenderTargetBase* render_target;
		};

		class AnimatedSpritesPass : public RenderPass
		{
		public:
			AnimatedSpritesPass(const AnimatedSpritePassCreationInfo& creationInfo);

			void update_scene_params(const SceneParams& params);
			void update_camera_params(const CameraParams& params);

			AnimatedSprite* add_sprite(rsl::unique_ptr<AnimatedSprite> sprite);

			void render(rex::gfx::RenderContext* renderCtx);

		private:
			RenderPassDesc create_desc(const AnimatedSpritePassCreationInfo& creationInfo) const;

			void init();

			void init_vb(rex::gfx::RenderContext* renderCtx);
			void init_render_info(rex::gfx::RenderContext* renderCtx);
			void init_ib(rex::gfx::RenderContext* renderCtx);
			//void init_tile_indices_uab();
			void init_shader_params();

		private:
			// The vertex buffer for a single tile
			// As we use instanced rendering, we only store 4 vertices
			rsl::unique_ptr<rex::gfx::VertexBuffer> m_tiles_vb_gpu;
			// The index buffer for a single tile
			// It's just 6 indices, allowing us to make a single quad
			rsl::unique_ptr<rex::gfx::IndexBuffer> m_tiles_ib_gpu;

			// The render info the shader needs for rendering
			// It's to store how big a single tile is, both on the tilemap 
			// as well as in the texture
			rsl::unique_ptr<rex::gfx::ConstantBuffer> m_tile_render_info;

			rex::gfx::RenderTargetBase* m_render_target;

			rsl::vector<rsl::unique_ptr<AnimatedSprite>> m_sprites;

			//SceneParams m_scene_params;
			//CameraParams m_camera_params;
		};
	}
}