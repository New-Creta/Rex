#pragma once

#include "rex_engine/engine/globals.h"

#include "rex_engine/assets/tilemap.h"
#include "rex_engine/assets/tileset_asset.h"

#include "rex_engine/gfx/resources/unordered_access_buffer.h"

//#include "rex_engine/gfx/rendering/render_pass.h"

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

		struct RenderRequest
		{
			RenderTarget* render_target;
		};

		struct TilemapRenderRequest : RenderRequest
		{
			const rex::Tilemap* tilemap;
			const rex::TilesetAsset* tileset;
		};

		struct TilemapRenderData
		{
			rsl::unique_ptr<rex::gfx::VertexBuffer> tiles_vb_gpu;
			rex::gfx::IndexBuffer* tiles_ib_gpu;
			rsl::unique_ptr<rex::gfx::ConstantBuffer> tile_render_info;
			rsl::unique_ptr<rex::gfx::UnorderedAccessBuffer> tile_indices_buffer;
		};

		class Renderer
		{
		public:
			Renderer();

		private:

		private:

		//	template <typename T, typename ... Args>
		//	T* add_render_pass(Args&& ... args)
		//	{
		//		rsl::unique_ptr<T> new_pass = rsl::make_unique<T>(rsl::forward<Args>(args)...);
		//		T* new_pass_ptr = new_pass.get();
		//		m_passes.emplace_back(rsl::move(new_pass_ptr));

		//		return new_pass_ptr;
		//	}

		//private:
		//	rsl::vector<rsl::unique_ptr<RenderPass>> m_passes;
		};
		namespace renderer
		{
			void init(globals::GlobalUniquePtr<Renderer> renderer);
			Renderer* instance();
			void shutdown();
		}
	}
}