#include "rex_renderer_core/rendering/render_pass.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/shader_reflection/shader_reflection_cache.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_renderer_core/system/shader_library.h"

#include "rex_renderer_core/system/gpu_engine.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogRenderPass);

		RenderPass::RenderPass(const RenderPassDesc& desc)
			: m_name(desc.name)
		{
			m_pso = rhi::create_pso(desc.pso_desc);
			ShaderPipelineReflection& reflection = shader_reflection_cache::load(desc.pso_desc.shader_pipeline);
			m_parameters_store = rsl::make_unique<ShaderParametersStore>(reflection.renderpass_param_store_desc);
			m_framebuffer = rsl::make_unique<FrameBuffer>(desc.framebuffer_desc);
		}

		void RenderPass::bind_to(RenderContext* ctx)
		{
			ctx->set_pipeline_state(m_pso.get());
			ctx->set_root_signature(m_pso->root_signature());
			ctx->set_primitive_topology(PrimitiveTopology::TriangleList);
			ctx->set_blend_factor(m_blend_factor);

			m_framebuffer->bind_to(ctx);

			const rsl::vector<rsl::unique_ptr<ShaderParameter>>& shader_params = m_parameters_store->params();
			for (const auto& shader_resource : shader_params)
			{
				shader_resource->bind_to(ctx);
			}
		}

		void RenderPass::set(rsl::string_view name, ConstantBuffer* constantBuffer)
		{
			m_parameters_store->set(name, constantBuffer);
		}
		void RenderPass::set(rsl::string_view name, Texture2D* texture)
		{
			m_parameters_store->set(name, texture);
		}
		void RenderPass::set(rsl::string_view name, Sampler2D* sampler)
		{
			m_parameters_store->set(name, sampler);
		}
		s32 RenderPass::slot(rsl::string_view name) const
		{
			return m_parameters_store->location(name).slot;
		}

		void RenderPass::set_blend_factor(const BlendFactor& blendFactor)
		{
			m_blend_factor = blendFactor;
		}

	}
}