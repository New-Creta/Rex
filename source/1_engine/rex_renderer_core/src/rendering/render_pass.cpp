#include "rex_renderer_core/rendering/render_pass.h"

#include "rex_renderer_core/gfx/rhi.h"

namespace rex
{
	namespace gfx
	{
		RenderPass::RenderPass(const RenderPassDesc& desc)
			: m_name(desc.name)
		{
			m_pso = rhi::create_pso(desc.pso_desc);
		}

		ScopedPoolObject<RenderContext> RenderPass::begin()
		{
			auto render_ctx = new_render_ctx(m_pso.get());
			render_ctx->set_root_signature(m_pso->root_signature());
			render_ctx->set_primitive_topology(PrimitiveTopology::TriangleList);

			return render_ctx;
		}
		void RenderPass::end(ScopedPoolObject<RenderContext>& ctx)
		{
				
		}

	}
}