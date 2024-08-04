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

			ShaderPipelineReflection reflection = reflect_shader_pipeline(desc.pso_desc.shader_pipeline);

			s32 param_idx = 0;
			for (const auto& cb : reflection.vs.constant_buffers())
			{
				m_shader_parameters.emplace(cb.name, ShaderParameter(param_idx++, ShdrParamType::ConstantBuffer));
			}
			for (const auto& cb : reflection.ps.constant_buffers())
			{
				m_shader_parameters.emplace(cb.name, ShaderParameter(param_idx++, ShdrParamType::ConstantBuffer));
			}
		}

		ScopedPoolObject<RenderContext> RenderPass::bind_resources()
		{
			auto render_ctx = new_render_ctx(m_pso.get());
			render_ctx->set_root_signature(m_pso->root_signature());
			render_ctx->set_primitive_topology(PrimitiveTopology::TriangleList); // should become part of the material

			auto copy_ctx = new_copy_ctx();
			for (auto& [name, param] : m_shader_parameters)
			{
				if (param.resource())
				{
					rsl::vector<ResourceView*> views = { static_cast<ConstantBuffer*>(param.resource())->resource_view() };
					auto start_handle = copy_ctx->copy_views(ViewHeapType::ConstantBufferView, views);
					render_ctx->set_graphics_root_descriptor_table(param.slot(), start_handle.get());
				}
			}

			return render_ctx;
		}

		void RenderPass::set(rsl::string_view key, ConstantBuffer* constantBuffer)
		{
			m_shader_parameters.at(key).set(constantBuffer);
		}

		s32 RenderPass::slot(rsl::string_view key) const
		{
			return m_shader_parameters.at(key).slot();
		}

	}
}