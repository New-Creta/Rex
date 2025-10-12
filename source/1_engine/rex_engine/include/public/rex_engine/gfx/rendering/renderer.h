#pragma once

#include "rex_engine/engine/globals.h"

#include "rex_engine/assets/tilemap.h"
#include "rex_engine/assets/tileset_asset.h"

#include "rex_engine/diagnostics/error.h"
#include "rex_engine/engine/globals.h"

#include "rex_engine/gfx/rendering/render_pass.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"
#include "rex_engine/gfx/resources/animated_sprite.h"

#include "rex_engine/gfx/rendering/render_passes/block_pass.h"
#include "rex_engine/gfx/rendering/render_passes/animated_sprites_pass.h"

namespace rex
{
	namespace gfx
	{
		class VertexBuffer;
		class IndexBuffer;
		class Material;
		class ConstantBuffer;
		class RenderTarget;

		// A drawlist holds data needed to draw a single element to the screen
		struct DrawList
		{
			Material* material;
			VertexBuffer* vb;   // The vertex buffer of the object
			IndexBuffer* ib;	// The index buffer of the object
			ConstantBuffer* per_instance_cb; // The constant buffer holding per instance data
		};

		// Renderer interface, very basic implementation and will likely be extended in the future
		class LegacyRenderer
		{
		public:
			virtual ~LegacyRenderer() = default;

			// Prepare the renderer for a new frame
			virtual void new_frame() = 0;

			// Render a new frame
			virtual void render() = 0;
		};

		class Renderer
		{
		public:
			template <typename Pass, typename ... Args>
			void add_new_pass(Args&& ... args)
			{
				m_passes.emplace_back(rsl::make_unique<Pass>(rsl::forward<Args>(args)...));

				return m_passes.back().get();
			}

			AnimatedSprite* add_animated_sprite(rsl::unique_ptr<AnimatedSprite> sprite);

		private:
			rsl::unique_ptr<AnimatedSpritesPass> m_animated_sprites_pass;
		};

		namespace renderer
		{
			Error init(globals::GlobalUniquePtr<Renderer> renderer);
			Renderer* instance();
			void shutdown();
		}



	}
}