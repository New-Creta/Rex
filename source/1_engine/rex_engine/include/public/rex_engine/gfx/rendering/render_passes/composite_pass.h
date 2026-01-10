#pragma once

#include "rex_engine/gfx/rendering/render_pass.h"

namespace rex
{
	namespace gfx
	{
		struct CompositePassCreationInfo
		{
			RenderTargetBase* src_render_target;
			RenderTargetBase* dst_render_target;
		};

		class CompositePass : public RenderPass
		{
		public:
			CompositePass(const CompositePassCreationInfo& creationInfo);

			void render(rex::gfx::RenderContext* renderCtx);

		private:
			RenderPassDesc create_desc(const CompositePassCreationInfo& creationInfo) const;

			void init();

			void init_vb(RenderContext* renderCtx);
			void init_ib(RenderContext* renderCtx);
			void init_shader_params();

		private:

			// The vertex buffer for a single tile
			// As we use instanced rendering, we only store 4 vertices
			rsl::unique_ptr<rex::gfx::VertexBuffer> m_tiles_vb_gpu;
			// The index buffer for a single tile
			// It's just 6 indices, allowing us to make a single quad
			rsl::unique_ptr<rex::gfx::IndexBuffer> m_tiles_ib_gpu;

			RenderTargetBase* m_src_render_target;
			RenderTargetBase* m_dst_render_target;
		};
	}
}