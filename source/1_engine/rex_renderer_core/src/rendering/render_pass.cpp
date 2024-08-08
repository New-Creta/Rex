#include "rex_renderer_core/rendering/render_pass.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/shader_reflection/shader_reflection_cache.h"

namespace rex
{
	namespace gfx
	{
		RenderPass::RenderPass(const RenderPassDesc& desc)
			: m_name(desc.name)
		{
			m_pso = rhi::create_pso(desc.pso_desc);
			ShaderPipelineReflection2& reflection = shader_reflection_cache::load(desc.pso_desc.shader_pipeline);
			m_parameters_store = rsl::make_unique<ShaderParametersStore>(reflection.renderpass_param_store_desc);
		}

		void RenderPass::bind_resources(RenderContext* ctx)
		{
			ctx->set_pipeline_state(m_pso.get());
			ctx->set_root_signature(m_pso->root_signature());
			ctx->set_primitive_topology(PrimitiveTopology::TriangleList);

			const rsl::vector<ShaderResource>& shader_resources = m_parameters_store->params();
			auto copy_ctx = new_copy_ctx();
			for (const auto& shader_resource : shader_resources)
			{
				rsl::unique_ptr<ResourceView> start_handle;
				switch (shader_resource.type())
				{
				case ShaderResourceType::ConstantBuffer: 
				{// Constant buffers are bound using gpu virtual address
					//start_handle = copy_ctx->copy_views(ViewHeapType::ConstantBufferView, shader_resource.views()); 
					ConstantBuffer* cb = reinterpret_cast<ConstantBuffer*>(shader_resource.views()[0]);
					if (cb)
					{
						ctx->set_constant_buffer(shader_resource.slot(), cb);
					}
					break;
				}
				case ShaderResourceType::Texture:        
					start_handle = copy_ctx->copy_views(ViewHeapType::ShaderResourceView, shader_resource.views());
					ctx->set_graphics_root_descriptor_table(shader_resource.slot(), start_handle.get());
					break;
				case ShaderResourceType::Sampler:        
					start_handle = copy_ctx->copy_views(ViewHeapType::Sampler, shader_resource.views());            
					ctx->set_graphics_root_descriptor_table(shader_resource.slot(), start_handle.get());
					break;
				}
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

	}
}