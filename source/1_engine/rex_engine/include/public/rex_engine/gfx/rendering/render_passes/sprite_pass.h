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

		// Make sure this is 32 bits as on the GPU it's also 32 bits
		enum class SpriteRenderBits : s32
		{
			None = 0,

			// If set, the sprite will be flipped, making left -> right and right -> left
			FlipX = BIT(1),
			// If set, the sprite will be flipped, making up -> down and down -> up
			FlipY = BIT(2),
			// If set, the sprite's bottom half will be rendered behind the background
			RenderBottomBehindBg = BIT(3),
		};

		// Info required to render a single sprite
		struct SpriteRenderProxy
		{
			// The texture holding the sprite to render
			const Texture2D* sprites_texture;

			// The area within the texture containing the sprite to render
			glm::vec2 uv_begin;
			rsl::point<s8> size;

			// Information where to draw the sprite
			PixelCoord pos;

			// Extra metadata to indicate how to render the sprite
			SpriteRenderBits render_bits;
		};

		class SpritePass : public RenderPass
		{
		public:
			explicit SpritePass(const AnimatedSpritePassCreationInfo& creationInfo);

			SpriteRenderProxy* add_sprite(rsl::unique_ptr<SpriteRenderProxy> sprite);

			void update_params(const SceneRenderParams& params);
			void render(RenderContext* renderCtx);

		private:
			RenderPassDesc create_desc(const AnimatedSpritePassCreationInfo& creationInfo) const;

			void init();

			void init_vb(RenderContext* renderCtx);
			void init_render_info();
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
			rsl::unique_ptr<StructuredBuffer> m_per_instance_sb;

			// The render target the pass will render to
			RenderTargetBase* m_render_target;

			// The background texture we're reading from when animated sprites are in grass
			// This background texture is the same as the render target
			rsl::unique_ptr<Texture2D> m_background_texture;

			// All the sprites we have to render
			rsl::vector<rsl::unique_ptr<SpriteRenderProxy>> m_sprites;

			// Parameters required for rendering
			SceneRenderParams m_params;
		};
	}
}