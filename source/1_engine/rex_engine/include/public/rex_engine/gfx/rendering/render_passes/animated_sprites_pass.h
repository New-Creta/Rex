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
			// The render target where the pass should render to
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
			rsl::unique_ptr<VertexBuffer> m_sprite_vb_gpu;
			
			// The index buffer for a single tile
			// It's just 6 indices, allowing us to make a single quad
			rsl::unique_ptr<IndexBuffer> m_sprite_ib_gpu;

			// The render info cbuffer is to give the shader information
			// about the screen and how big pixels of our sprites should be presented on screen
			rsl::unique_ptr<ConstantBuffer> m_screen_info_cbuffer;

			// the structured buffered we hold per animated sprite instance
			constexpr static s32 s_max_allowed_instances = 16;
			rsl::unique_ptr<StructuredBuffer> m_per_instance_buffer_array;

			// The render target the pass will render to
			RenderTargetBase* m_render_target;

			// The background texture we're reading from when animated sprites are in grass
			// This background texture is the same as the render target
			rsl::unique_ptr<Texture2D> m_background_texture;

			// All the sprites we have to render
			rsl::vector<rsl::unique_ptr<AnimatedSprite>> m_sprites;

			// Parameters required for rendering
			SceneRenderParams m_params;
		};
	}
}