#pragma once

#include "rex_engine/gfx/rendering/render_pass.h"
#include "rex_engine/gfx/rendering/render_params.h"

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

			AnimatedSprite* add_sprite(rsl::unique_ptr<AnimatedSprite> sprite);

			void update_params(const SceneRenderParams& params);
			void render(RenderContext* renderCtx);

		private:
			RenderPassDesc create_desc(const AnimatedSpritePassCreationInfo& creationInfo) const;

			void init();

			void init_vb(RenderContext* renderCtx);
			void init_render_info(RenderContext* renderCtx);
			void init_ib(RenderContext* renderCtx);
			void init_shader_params();

		private:
			// The vertex buffer for a single tile
			// As we use instanced rendering, we only store 4 vertices
			rsl::unique_ptr<VertexBuffer> m_tiles_vb_gpu;
			// The index buffer for a single tile
			// It's just 6 indices, allowing us to make a single quad
			rsl::unique_ptr<IndexBuffer> m_tiles_ib_gpu;

			// The render info cbuffer is to give the shader information
			// about the screen and how big pixels of our sprites should be presented on screen
			rsl::unique_ptr<ConstantBuffer> m_screen_info_cbuffer;

			rsl::unique_ptr<ConstantBuffer> m_per_instance_cbuffer;

			RenderTargetBase* m_render_target;
			rsl::unique_ptr<Texture2D> m_render_target_as_texture;

			rsl::vector<rsl::unique_ptr<AnimatedSprite>> m_sprites;

			SceneRenderParams m_params;
		};
	}
}